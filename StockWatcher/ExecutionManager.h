#pragma once
#include "DBStore.h"
#include <unordered_map>

class ExecutionManager
{
public:
	ExecutionManager(DBStore& sw);
	
	HANDLE AssignEvent(std::string iD, bool bStart = false);
	
	HANDLE AssignExecutor(std::string iD);
	
	void SetContext(ExecutionContext e);
	
private:
	DBStore m_watcher;
	std::unordered_map<std::string, HANDLE> m_ExecutorMap;
	std::unordered_map<std::string, HANDLE> m_EventMap;
	std::unordered_map<std::string, HANDLE> m_SEventMap;
};

