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

using namespace std;

class ExecutionEngine
{
public:
	ExecutionEngine(StockWatcher& sw)
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
		executionContext.m_handle = em->AssignExecutor(executionContext.m_StockID);
		executionContext.m_hEvent = em->AssignEvent(executionContext.m_StockID);
		executionContext.m_hStartEvent = em->AssignEvent(executionContext.m_StockID,true);
		em->SetContext(executionContext);
	}

	void Analyze(ExecutionContext t)
	{
		cout << "\nAnalysing Stock : " << t.m_StockID;
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
		{1},{2},{3},{4},{5}
	};

	StockWatcher sw;
	ExecutionEngine e(sw);
	for (size_t i = 0; i < vContexts.size(); i++)
	{
		e.SetContext(vContexts[i]);
	}

	for (size_t i = 0; i < vContexts.size(); i++)
	{
		e.SetContext(vContexts[i]);
	}
	char ch;
	cin >> ch;
	return 0;
}