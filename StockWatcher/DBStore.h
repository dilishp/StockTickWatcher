#pragma once
#include "ExecutionContext.h"

class DBStore
{
public:
	void SetContext(ExecutionContext& e);

	static unsigned int _stdcall OnExecute(void* p_This);
	
	void WriteToInfluxDB(ExecutionContext& context);

	long long getCurrentTimestampNs();
private:
	ExecutionContext m_context;
};
