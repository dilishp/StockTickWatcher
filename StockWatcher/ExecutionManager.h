#pragma once
#include "DBStore.h"
#include <unordered_map>

class ExecutionManager
{
public:
	ExecutionManager(StockWatcher& sw);
	
	HANDLE AssignEvent(int iD, bool bStart = false);
	
	HANDLE AssignExecutor(int iD);
	
	void SetContext(ExecutionContext e);
	
private:
	StockWatcher m_watcher;
	std::unordered_map<int, HANDLE> m_ExecutorMap;
	std::unordered_map<int, HANDLE> m_EventMap;
	std::unordered_map<int, HANDLE> m_SEventMap;
};

