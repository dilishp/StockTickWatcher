#include "DBStore.h"
#include <iostream>
#include <chrono>
#include <sstream>
#include <curl/curl.h>
#include <mutex>
#include <nlohmann/json.hpp>

using namespace std;

std::mutex m;

void DBStore::SetContext(ExecutionContext& e)
{
	m_context = e;

	SetEvent(m_context.m_hStartEvent);

	WaitForSingleObject(m_context.m_hEvent, INFINITE);
}

// Helper function to get current time in nanoseconds since epoch
long long DBStore::getCurrentTimestampNs()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}


std::string INFLUXDB_URL = "http://localhost:8086/api/v2/buckets";
std::string INFLUXDB_TOKEN = "uVXjgEZFa_9zD05gHbpUUxDl8gQ2ZqTHXYP1vbBoyj_mLC7SdZuQcHyBu2RgzYBkfwwNKwL1-G-99hyeLl8PKg==";  // Replace with your token
std::string ORG_ID = "f0cc73ec84f9041f";              // Replace with your org ID
std::string ORG_NAME = "CatchyBytes";              // Replace with your org ID


// Callback function to capture response from libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to check if a bucket exists
static bool BucketExists(const std::string& bucketName) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: Token " + INFLUXDB_TOKEN).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, INFLUXDB_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    try {
        // Parse JSON response
        auto jsonResponse = nlohmann::json::parse(response);

        // Check if the bucket exists
        for (const auto& bucket : jsonResponse["buckets"]) {
            if (bucket["name"] == bucketName) {
                return true;
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
    }

    return false;
}
static bool CreateInfluxDBBucket(const std::string& bucketName, int retentionSeconds = 0) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return false;
    }

    // Construct JSON request body
    std::string jsonData = "{ \"orgID\": \"" + ORG_ID + "\", \"name\": \"" + bucketName + "\"";
    if (retentionSeconds > 0) {
        jsonData += ", \"retentionRules\": [{ \"type\": \"expire\", \"everySeconds\": " + std::to_string(retentionSeconds) + " }]";
    }
    jsonData += " }";

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("Authorization: Token " + INFLUXDB_TOKEN).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, INFLUXDB_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return false;
    }

    // Cleanup
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    return true;
}

// Function to write data to InfluxDB using libcurl.
void DBStore::WriteToInfluxDB(ExecutionContext& context)
{
    // InfluxDB connection parameters
    if (!BucketExists(context.m_stockTicker))
    {
        bool bCreated = CreateInfluxDBBucket(context.m_stockTicker);
        if (!bCreated)
        {
            cerr << "\n[curl error] " << "Unable to create Bucket";
        }
    }

    const string url = "http://localhost:8086/api/v2/write?org="+ORG_NAME+"&bucket="+context.m_stockTicker+"&precision=s";

    // Prepare line protocol data.
    // Example: measurement,tag=value field=value timestamp
    // Here we use "stock" as the measurement and use stockID as a tag.
    long long timestamp = getCurrentTimestampNs();
    ostringstream lineData;
    lineData << "stock_candles,symbol=" << context.m_stockTicker << " open=" << context.m_open
        <<",high="<<context.m_high<<",low="<<context.m_low<<",close="<<context.m_close<<" "<<timestamp;
    string data = lineData.str();

    CURL* curl = curl_easy_init();
    if (curl)
    {
        struct curl_slist* headers = NULL;
        std::string authorization = ("Authorization: Token " + INFLUXDB_TOKEN);
        headers = curl_slist_append(headers, authorization.c_str());
        headers = curl_slist_append(headers, "Content-Type: text/plain");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set POST data.
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        // Optionally, set a timeout
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            cerr << "\n[curl error] " << curl_easy_strerror(res) << "\n";
        }
        curl_easy_cleanup(curl);
    }
    else
    {
        cerr << "\nFailed to initialize curl.\n";
    }
}

unsigned int _stdcall DBStore::OnExecute(void* p_This)
{
	DBStore* sw = static_cast<DBStore*>(p_This);
	while (true)
	{
		WaitForSingleObject(sw->m_context.m_hStartEvent, INFINITE);

        sw->WriteToInfluxDB(sw->m_context);

		SetEvent(sw->m_context.m_hEvent);

	}
	return 1;
}
