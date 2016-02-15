#include "HiDBImpl.h"

#include "../Common/HiCritical.h"


static void defaultFun(HiDBLogType type,const char* ex)
{
}

HiDBImpl::HiDBImpl(bool isUsingLock)
{
	this->m_pCritical = NULL;
	if (isUsingLock)
	{
		this->m_pCritical = new HiCritical();
	}

	logFun = defaultFun;
}

HiDBImpl::~HiDBImpl()
{
	if (this->m_pCritical)
	{
		delete this->m_pCritical;
		this->m_pCritical = NULL;
	}
}
void HiDBImpl::SetLogFunction(const HiDBLogFun& fun)
{
	logFun = fun;
}