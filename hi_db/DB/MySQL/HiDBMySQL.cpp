#include "HiDBMySQL.h"

#include "../../common/HiDBMacro.h"
#include "../../common/HiCritical.h"

using namespace std;

// 构造函数
HiDBMySQL::HiDBMySQL(bool isUsingLock): HiDBImpl(isUsingLock), m_pSQLData(NULL)
{
}

// 析构函数
HiDBMySQL::~HiDBMySQL()
{
	if(this->m_pSQLData)
	{
		::mysql_close(this->m_pSQLData);
		this->m_pSQLData = NULL;
	}
}

// 打开数据库连接
bool HiDBMySQL::Open(const char* conn)
{
	HiCriticalMng msg(this->m_pCritical);

	if(this->m_pSQLData)
	{
		return true;
	}

	if (::strlen(conn) == 0)
	{
		return false;
	}

	char host[40]	= {0};
	char dbname[40] = {0};
	long port		= 0;
	char user[40]	= {0};
	char pwd[40]	= {0};
	char chrSet[40]	= {0};

	::sscanf(conn, 
		("host=%[^;];port=%d;dbname=%[^;];user=%[^;];pwd=%[^;];charset=%[^;];"),
		host, &port, dbname,user, pwd, chrSet);

	::mysql_init(&this->m_SQLData);

	if(::mysql_real_connect(&this->m_SQLData, 
		host, user, pwd, dbname, port, NULL, 0))
	{
		this->m_pSQLData = &this->m_SQLData;

		ostringstream oss;
		oss<<"set names "<<chrSet;

		::mysql_query(this->m_pSQLData, oss.str().c_str());

		this->m_ConnString = conn;

		return true;
	}
	else
	{
		if(this->m_pSQLData)
		{
			long id = mysql_errno(this->m_pSQLData);
			const char* err = mysql_error(this->m_pSQLData);
			//关闭连接
			::mysql_close(this->m_pSQLData);

			this->m_pSQLData = NULL;

			HiDBHelperOnError("Open(const char*)", err, "", id);
		}
		return false;
	}
}

void HiDBMySQL::Close(void)
{
	HiCriticalMng msg(this->m_pCritical);

	if(this->m_pSQLData)
	{
		::mysql_close(this->m_pSQLData);
		this->m_pSQLData = NULL;
	}
}

bool HiDBMySQL::ExecuteNoQuery(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		//HiDBHelperOnError("ExecuteNoQuery(const char*)", "Database is not open", sql, 0);
		return false;
	}

	long error = ::mysql_query(this->m_pSQLData, sql);
	bool result = (error == 0)? true:false;
	if (result)
	{
		MYSQL_RES *pResult = ::mysql_store_result(this->m_pSQLData);
		if(pResult)
		{
			::mysql_free_result(pResult);	//释放数据集
		}
		return true;
	}

	unsigned int errId = mysql_errno(this->m_pSQLData);
	if (errId == 0)
	{
		MYSQL_RES *pResult = ::mysql_store_result(this->m_pSQLData);
		if(pResult)
		{
			::mysql_free_result(pResult);	//释放数据集
		}
		return false;
	}
	HiDBHelperOnError("ExecuteNoQuery(const char*)", 
		mysql_error(this->m_pSQLData), sql, mysql_errno(this->m_pSQLData));
}

string HiDBMySQL::ExecuteScalar(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		//HiDBHelperOnError("ExecuteNoQuery(const char*)", "Database is not open", sql, 0);
		return "";
	}

	long error = ::mysql_query(this->m_pSQLData, sql);
	if(error != 0)//执行SQL语句
	{
		HiDBHelperOnError("HiDBMySQL::ExecuteScalar(const char*, HiDBValue&)", 
			::mysql_error(this->m_pSQLData), sql, error);
	}

	MYSQL_RES* pResult= ::mysql_store_result(this->m_pSQLData);	//获取数据集
	if(!pResult)
	{
		HiDBHelperOnError("HiDBMySQL::ExecuteScalar(const char*, HiDBValue&)", 
			::mysql_error(this->m_pSQLData), sql, mysql_errno(this->m_pSQLData));
	}
	if (pResult->row_count == 0)
	{
		::mysql_free_result(pResult);	//释放数据集
		return "";
	}

	MYSQL_FIELD* pFields = ::mysql_fetch_field(pResult);	
	::mysql_data_seek(pResult, 0);

	MYSQL_ROW curRow = ::mysql_fetch_row(pResult);

	string ret = curRow[0];

	::mysql_free_result(pResult);	//释放数据集
	return ret;
}

std::shared_ptr<HiDBTable> HiDBMySQL::ExecuteQuery(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		//HiDBHelperOnError("ExecuteNoQuery(const char*)", "Database is not open", sql, 0);
		return NULL;
	}

	long error = ::mysql_query(this->m_pSQLData, sql);
	if(error != 0)//执行SQL语句
	{
		HiDBHelperOnError("HiDBMySQL::ExecuteQuery(const char*)", 
			::mysql_error(this->m_pSQLData), sql, error);
	}

	MYSQL_RES* pResult= ::mysql_store_result(this->m_pSQLData);	//获取数据集
	if(!pResult)
	{
		HiDBHelperOnError("HiDBMySQL::ExecuteQuery(const char*)", 
			::mysql_error(this->m_pSQLData), sql, mysql_errno(this->m_pSQLData));
	}

	std::shared_ptr<HiDBTable> tb(new HiDBTable());

	if (pResult->row_count == 0)
	{
		::mysql_free_result(pResult);	//释放数据集
		return tb;
	}

	MYSQL_FIELD* pFields = ::mysql_fetch_field(pResult);	

	for (int i = 0; i < pResult->row_count; i++)
	{
		::mysql_data_seek(pResult, i);
		MYSQL_ROW curRow = ::mysql_fetch_row(pResult);

		map<string, string> list;
		tb->push_back(list);
		map<string, string>& list2 = *tb->rbegin();
		for (int j = 0; j < (long)pResult->field_count; j++)
		{
			string val = "";
			if (curRow[j])
			{
				val = curRow[j];
			}
			list2[pFields[j].name] = val;
		}
	}

	::mysql_free_result(pResult);	//释放数据集

	return tb;
}	

void HiDBMySQL::OnTransaction(const std::function<void()>& fun)
{
	HiCriticalMng msg(this->m_pCritical);

	mysql_autocommit(this->m_pSQLData, 0);

	try
	{
		fun();

		if (::mysql_commit(this->m_pSQLData) == 0)
		{
			mysql_autocommit(this->m_pSQLData, 1);
			return;
		}

		if (::mysql_rollback(this->m_pSQLData) == 0)
		{
			mysql_autocommit(this->m_pSQLData, 1);
		}

		HiDBHelperOnError_void("HiDBMySQL::OnTransaction", 
			"execute transaction sucess,but commit failed", "", 0);
	}
	catch(HiDBException& e)
	{
		if (::mysql_rollback(this->m_pSQLData) == 0)
		{
			mysql_autocommit(this->m_pSQLData, 1);
		}
		HiDBHelperOnError_void("HiDBMySQL::RollbackTransaction", 
			e.m_descript, e.m_sql, e.m_errorId);
	}
	catch (...)
	{
		if (::mysql_rollback(this->m_pSQLData) == 0)
		{
			mysql_autocommit(this->m_pSQLData, 1);
		}
		HiDBHelperOnError_void("HiDBMySQL::RollbackTransaction", 
			::mysql_error(this->m_pSQLData), "", mysql_errno(this->m_pSQLData));
	}
}

bool HiDBMySQL::IsOpen(void)
{
	return this->m_pSQLData == NULL ? false : true;
}