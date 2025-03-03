#pragma once
#include "ExecutionContext.h"

class StockWatcher
{
public:
	void SetContext(ExecutionContext& e);

	static unsigned int _stdcall OnExecute(void* p_This);
	
private:
	ExecutionContext m_context;
};
