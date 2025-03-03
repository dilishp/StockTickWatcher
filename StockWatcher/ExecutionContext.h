#pragma once
#include <Windows.h>
#include <queue>
#include <string>

class ExecutionContext
{
public:
	ExecutionContext()
	{

	}
	ExecutionContext(std::string s,unsigned int o,unsigned int c,unsigned int l,unsigned int h)
	{
		m_stockTicker = s;
		m_open = o;
		m_close = c;
		m_high = h;
		m_low = l;
	}
	std::string m_stockTicker;
	unsigned int m_open;
	unsigned int m_close;
	unsigned int m_low;
	unsigned int m_high;
	long long m_timeStamp;
	HANDLE m_handle;
	HANDLE m_hEvent;
	HANDLE m_hStartEvent;
	std::queue<int> m_tickerQueue;
};