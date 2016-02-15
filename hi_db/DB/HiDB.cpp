#include <stdarg.h>
#include "DB/HiDB.h"
#include "HiDBFactory.h"
#include "HiDBImpl.h"
#include "../common/HiDBMacro.h"

using namespace std;

#if !defined(HISDB_ON_VARLIST)
#define HISDB_ON_VARLIST(x, y) \
	char chArr[2048] = {0};\
	char* pchar = &chArr[0];\
	va_list pArgList;\
	va_start(pArgList, y);\
	::_vsnprintf(chArr, 2047, x, pArgList);	\
	va_end(pArgList) ;\
	logFun(HiDLT_SQL, chArr);
#endif


static void defaultFun(HiDBLogType type,const char* ex)
{
}

static bool IsImplOK(HiDBImpl* db)
{
	if (!db)
	{
		return false;
	}
	return true;
}

// 构造函数
HiDB::HiDB(HiDBType type, bool isUsingLock):m_Impl(NULL)
{
	this->m_Impl = NS_HiDB::CreateDBImpl(type, isUsingLock);

	logFun = defaultFun;
}

// 析构函数
HiDB::~HiDB()
{
	if (this->m_Impl)
	{
		delete this->m_Impl;
		this->m_Impl = NULL;
	}
}

// 打开数据库连接
bool HiDB::Open(const char* conn)
{
	if (!IsImplOK(this->m_Impl))
	{
		return false;
	}

	return this->m_Impl->Open(conn);
}

bool HiDB::IsOpen()
{
	if (!IsImplOK(this->m_Impl))
	{
		return false;
	}

	return this->m_Impl->IsOpen();
}

void HiDB::Close(void)
{
	if (!IsImplOK(this->m_Impl))
	{
		return;
	}

	return this->m_Impl->Close();
}

bool HiDB::ExecuteNoQuery(const char* sql, ...)
{
	bool bRet = false;

	if (!IsImplOK(this->m_Impl))
	{
		return false;
	}

	HISDB_ON_VARLIST(sql, sql);

	try
	{
		bRet = this->m_Impl->ExecuteNoQuery(chArr);
	}
	catch (HiDBException e)
	{
		return false;
	}

	return bRet;
}

string HiDB::ExecuteScalar(const char* sql, ...)
{
	if (!IsImplOK(this->m_Impl))
	{
		return "";
	}

	HISDB_ON_VARLIST(sql, sql);
	
	return this->m_Impl->ExecuteScalar(chArr);
}

std::shared_ptr<HiDBTable> HiDB::ExecuteQuery(const char* sql, ...)
{
	if (!IsImplOK(this->m_Impl))
	{
		return NULL;
	}
	HISDB_ON_VARLIST(sql, sql);

	return this->m_Impl->ExecuteQuery(chArr);
}	

void HiDB::OnTransaction(const std::function<void()>& fun)
{
	if (!IsImplOK(this->m_Impl))
	{
		HiDBHelperOnError_void("HiDB::OnTransaction", 
			"HiDB is not impl", "", 0);
	}
	return this->m_Impl->OnTransaction(fun);
}

void HiDB::SetLogFunction(const HiDBLogFun& fun)
{
	logFun = fun;
	if (IsImplOK(this->m_Impl))
	{
		this->m_Impl->SetLogFunction(fun);
	}
}