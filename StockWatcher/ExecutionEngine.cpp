#include <iostream>
#include <vector>
#include <thread>
#include <Windows.h>
#include <process.h>
#include <unordered_map>
#include <queue>
#include <mutex>
#include "DBStore.h"
#include "ExecutionContext.h"
#include "ExecutionManager.h"
#include <curl/curl.h>

using namespace std;

class ExecutionEngine
{
public:
	ExecutionEngine(DBStore& sw)
	{
		em = new ExecutionManager(sw);
	}
	void SetContext(std::vector<ExecutionContext>& vExecutionContexts)
	{
		m_Contexts = std::move(vExecutionContexts);
	}

	void SetContext(ExecutionContext executionContext)
	{
		//Context set is called
		executionContext.m_handle = em->AssignExecutor(executionContext.m_stockTicker);
		executionContext.m_hEvent = em->AssignEvent(executionContext.m_stockTicker);
		executionContext.m_hStartEvent = em->AssignEvent(executionContext.m_stockTicker,true);
		em->SetContext(executionContext);
	}

	void Analyze(ExecutionContext t)
	{
		cout << "\nAnalysing Stock : " << t.m_stockTicker;
	}

	void Execute()
	{
		for (int nStock = 0; nStock < m_Contexts.size(); ++nStock)
		{
			ExecutionContext e = m_Contexts[nStock];
			std::thread t(&ExecutionEngine::Analyze,this,e);
			t.detach();
			m_executors.emplace_back(std::move(t));
		}
	}

private:
	std::vector<std::thread> m_executors;
	std::vector<ExecutionContext> m_Contexts;
	ExecutionManager* em;
};

int main()
{
	std::vector<ExecutionContext> vContexts =
	{
		ExecutionContext("AAPL",12.5,18,24,10.3),
		ExecutionContext("BPL",102.5,180,240,100.3),
		ExecutionContext("KLAC",112.5,118,214,110.3),
		ExecutionContext("NVD",122.5,128,224,120.3),
		ExecutionContext("PIND",132.5,138,234,130.3),
		ExecutionContext("JFN",142.5,148,244,140.3),
		ExecutionContext("JSN",142.5,148,244,140.3),
		ExecutionContext("RDH",142.5,148,244,140.3)
	};

	// Initialize libcurl once.
	curl_global_init(CURL_GLOBAL_ALL);

	DBStore sw;
	ExecutionEngine e(sw);
	for (size_t i = 0; i < vContexts.size(); i++)
	{
		e.SetContext(vContexts[i]);
		Sleep(1000);
	}

	for (size_t i = 0; i < vContexts.size(); i++)
	{
		e.SetContext(vContexts[i]);
		Sleep(1000);
	}

	curl_global_cleanup();
	char ch;
	cin >> ch;
	return 0;
}