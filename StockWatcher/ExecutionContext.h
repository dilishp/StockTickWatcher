#pragma once
#include <Windows.h>
#include <queue>

class ExecutionContext
{
public:
	int m_StockID;
	HANDLE m_handle;
	HANDLE m_hEvent;
	HANDLE m_hStartEvent;
	std::queue<int> m_tickerQueue;
};