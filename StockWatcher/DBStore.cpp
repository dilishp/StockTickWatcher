#include "DBStore.h"
#include <iostream>


void StockWatcher::SetContext(ExecutionContext& e)
{
	m_context = e;

	SetEvent(m_context.m_hStartEvent);

	WaitForSingleObject(m_context.m_hEvent, INFINITE);
}

unsigned int _stdcall StockWatcher::OnExecute(void* p_This)
{
	StockWatcher* sw = static_cast<StockWatcher*>(p_This);
	while (true)
	{
		WaitForSingleObject(sw->m_context.m_hStartEvent, INFINITE);

		fprintf(stderr, "\nHello from thread! (ThreadID: %u)\n", GetCurrentThreadId());

		std::cout << "\nStock Watcher Running for Stock : " << sw->m_context.m_StockID;

		SetEvent(sw->m_context.m_hEvent);

	}
	return 1;
}
