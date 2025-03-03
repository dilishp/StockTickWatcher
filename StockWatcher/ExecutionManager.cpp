#include "ExecutionManager.h"
#include <process.h>


ExecutionManager::ExecutionManager(DBStore& sw)
{
	m_watcher = sw;
}
	
HANDLE ExecutionManager::AssignEvent(std::string iD, bool bStart)
{
	if (!bStart)
	{
		if (m_EventMap.find(iD) != m_EventMap.end())
		{
			return m_EventMap[iD];
		}
		else
		{
			//manual reset event
			m_EventMap[iD] = CreateEvent(NULL, FALSE, FALSE, NULL);
			return m_EventMap[iD];
		}
	}
	else
	{
		if (m_SEventMap.find(iD) != m_SEventMap.end())
		{
			return m_SEventMap[iD];
		}
		else
		{
			//manual reset event
			m_SEventMap[iD] = CreateEvent(NULL, FALSE, FALSE, NULL);
			return m_SEventMap[iD];
		}
	}
}

HANDLE ExecutionManager::AssignExecutor(std::string iD)
{
	if (m_ExecutorMap.find(iD) != m_ExecutorMap.end())
	{
		return m_ExecutorMap[iD];
	}
	else
	{
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &DBStore::OnExecute, &m_watcher, NULL, NULL);
		if (!hThread)
		{
			fputs("Failed to create thread!\n", stderr);
			return NULL;
		}
		m_ExecutorMap[iD] = hThread;
		return hThread;
	}
}

void ExecutionManager::SetContext(ExecutionContext e)
{
	m_watcher.SetContext(e);
}

