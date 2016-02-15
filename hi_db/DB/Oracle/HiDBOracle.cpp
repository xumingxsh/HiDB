#include "HiDBOracle.h"

#include <time.h> 
#include "../../common/HiDBMacro.h"
#include "../../common/HiCritical.h"
#include "../../common/HisScopeGuard.h"

using namespace std;
using namespace ADOCG;

class OracleProxy
{
public:
	OracleProxy(): is_init_(false)
	{

	}
	~OracleProxy()
	{
		if (is_init_)
		{
			//::CoUninitialize();
		}
	}
public:
	void on_init()
	{
		if (is_init_)
		{
			return;
		}
		is_init_ = true;
		::CoInitialize(NULL);
	}
private:
	bool is_init_;
};

static OracleProxy s_Proxy;


// 构造函数
HiDBOracle::HiDBOracle(bool isUsingLock): HiDBImpl(isUsingLock), m_pConnection(NULL)
{
	s_Proxy.on_init();
	m_pConnection.CreateInstance( __uuidof(Connection) );
}

// 析构函数
HiDBOracle::~HiDBOracle()
{
	Close();
	if (m_pConnection != NULL)
	{
		m_pConnection->Release();
		m_pConnection.Detach();
	}
	m_pConnection = NULL;
}

// 打开数据库连接
bool HiDBOracle::Open(const char* conn)
{
	::CoInitialize(NULL);
	HiCriticalMng msg(this->m_pCritical);
	
	if (!m_pConnection || ::strlen(conn) == 0)
	{
		return false;
	}

	if (IsOpen())
	{
		m_pConnection->Close();
	}
	try
	{
		HRESULT hr = m_pConnection->Open(_bstr_t(conn), _bstr_t(""), _bstr_t(""), NULL);
		return hr == S_OK;
	}
	catch (_com_error &e)
	{
		on_excetion("HiDBOracle::Open", "", e);
		return false;
	}
}

void HiDBOracle::Close(void)
{
	HiCriticalMng msg(this->m_pCritical);

	if( IsOpen() )
	{
		m_pConnection->Close();
	}
}

bool HiDBOracle::ExecuteNoQuery(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		return false;
	}

	_variant_t vtRecords;

	try
	{
		m_pConnection->Execute(_bstr_t(sql), &vtRecords, adExecuteNoRecords);
		return true;
	}
	catch (_com_error &e)
	{
		on_excetion("HiDBOracle::ExecuteNoQuery", sql, e);
		return false;
	}
}

static string var2str(_variant_t&);
string HiDBOracle::ExecuteScalar(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);
	
	if (!this->IsOpen())
	{
		return "";
	}
	_RecordsetPtr  rs;
	create_rs("HiDBOracle::ExecuteScalar", sql, rs);

	rs->PutCursorLocation(adUseClient);
	ON_SCOPE_EXIT([&]{
		rs->Close();
		rs->Release();
		rs.Detach();
		rs = NULL;
	});

	_variant_t vtRecords;

	try
	{
		rs->Open(sql, _variant_t((IDispatch*)m_pConnection, true),
			adOpenStatic, adLockOptimistic, adCmdText);
		if (rs->GetRecordCount() <= 0)
		{
			return "";
		}
		rs->MoveFirst();
		
		_variant_t vtFld;
		_variant_t vtIndex;

		vtIndex.vt = VT_I2;
		vtIndex.iVal = 0;
		vtFld = rs->Fields->GetItem(vtIndex)->Value;
		string ret = var2str(vtFld);
		
		return ret;
	}
	catch (_com_error &e)
	{
		on_excetion("HiDBOracle::ExecuteScalar", sql, e);
		return "";
	}
}

std::shared_ptr<HiDBTable> HiDBOracle::ExecuteQuery(const char* sql)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		//HiDBHelperOnError("ExecuteNoQuery(const char*)", "Database is not open", sql, 0);
		return NULL;
	}

	_RecordsetPtr  rs;
	create_rs("HiDBOracle::ExecuteQuery", sql, rs);

	ON_SCOPE_EXIT([&]{
		rs->Close();
		rs.Release();
		rs.Detach();
		rs = NULL;
	});

	std::shared_ptr<HiDBTable> tb(new HiDBTable());
	try
	{
		rs->Open(sql, _variant_t((IDispatch*)m_pConnection, true),
			adOpenStatic, adLockOptimistic, adCmdText);
		if (rs->GetRecordCount() <= 0)
		{
			return tb;
		}

		_variant_t vtRecords;
		rs->MoveFirst();
		int cols_count = rs->GetFields()->Count;
		while(VARIANT_TRUE != rs->ADOBOF && VARIANT_TRUE != rs->ADOEOF)
		{

			map<string, string> list;
			tb->push_back(list);
			map<string, string>& list2 = *tb->rbegin();
			for (int i = 0; i < cols_count; i++)
			{
				_variant_t vtFld;
				_variant_t vtIndex;

				vtIndex.vt = VT_I2;
				vtIndex.iVal = i;
				vtFld = rs->Fields->GetItem(vtIndex)->Value;
				
				string key = rs->Fields->GetItem(vtIndex)->Name;
				list2[key] = var2str(vtFld);
			}
			rs->MoveNext();
		}
	}
	catch (_com_error &e)
	{	
		on_excetion("HiDBOracle::ExecuteQuery", sql, e);
	}
	return tb;
}	

void HiDBOracle::OnTransaction(const std::function<void()>& fun)
{
	HiCriticalMng msg(this->m_pCritical);

	if (!this->IsOpen())
	{
		HiDBHelperOnError("OnTransaction()", "Database is not open", "", 0);
	}

	bool is_rool_back_failed = false;
	try
	{
		m_pConnection->BeginTrans();
		fun();

		if (m_pConnection->CommitTrans() == 0)
		{
			return;
		}

		if (m_pConnection->RollbackTrans() != 0)
		{
			is_rool_back_failed = true;
		}

	}
	catch(HiDBException& e)
	{
		if (m_pConnection->RollbackTrans() != 0)
		{
			HiDBHelperOnError_void("HiDBOracle::OnTransaction", 
				"execute transaction  failed,and rollback failed", "", 0);
		}

		HiDBHelperOnError_void("HiDBOracle::RollbackTransaction", 
			e.m_descript, e.m_sql, e.m_errorId);
	}
	catch (...)
	{
		if (m_pConnection->RollbackTrans() != 0)
		{
			HiDBHelperOnError_void("HiDBOracle::OnTransaction", 
				"execute transaction failed", "", 0);
		}
		HiDBHelperOnError_void("HiDBOracle::RollbackTransaction", 
			"execute transaction failed,and rollback failed", "", 0);
	}

	if (is_rool_back_failed)
	{
		HiDBHelperOnError_void("HiDBOracle::OnTransaction", 
			"execute transaction failed, rollback failed", "", 0);
	}

	HiDBHelperOnError_void("HiDBOracle::OnTransaction", 
		"execute transaction failed", "", 0);
}

bool HiDBOracle::IsOpen(void)
{
	if (m_pConnection)
	{
		return m_pConnection->GetState() != adStateClosed;
	}
	else
	{
		return false;
	}
}

void HiDBOracle::on_excetion(const char* name, const char* sql, _com_error &e)
{
	_bstr_t bstrSource(e.Source());
	_bstr_t bstrDescription(e.Description());
	stringstream oss;
	oss<<"source:"<<bstrSource<<"\nDescription:"<<bstrDescription<<"\nerror message:"<<e.ErrorMessage();
	HiDBHelperOnError("HiDBOracle::ExecuteQuery", oss.str().c_str(), sql, e.Error());	
}

void HiDBOracle::create_rs(const char* name, const char* sql, _RecordsetPtr& rs)
{
	::CoInitialize(NULL);
	HRESULT hr = rs.CreateInstance(__uuidof(Recordset));
	if (FAILED(hr))
	{		
		_com_error e(hr);		
		on_excetion("HiDBOracle::ExecuteScalar", sql, e);
	}
	rs->PutCursorLocation(adUseClient);
}


template<typename T>
static string N2S(T v)
{
	stringstream oss;
	oss<<v;
	return oss.str();
}

static long DecimalToLong(_variant_t& vtValue)
{
	long lret = 0;
	VarI4FromDec(&(vtValue.decVal), &lret);
	return lret;
}

static double DecimalToDouble( _variant_t& vtValue)
{
	double dbReturn = 0.0;
	VarR8FromDec(&(vtValue.decVal), &dbReturn);

	return dbReturn;
}

string var2str(_variant_t& vtFld)
{
	switch(vtFld.vt) 
	{
	case VT_R4://float
		{
			return N2S(vtFld.fltVal);
		}
	case VT_R8://double
		{
			return N2S(vtFld.dblVal);
		}
	case VT_BSTR://str
		{
			string str;
			str.assign(bstr_t(vtFld.bstrVal));
			return str;
		}
	case VT_I2://short
		{
			return N2S(vtFld.iVal);
		}
	case VT_UI1://byte
		{
			return N2S(vtFld.bVal);
		}
	case VT_INT://int
		{
			return N2S(vtFld.intVal);
		}
	case VT_I4://long
		{
			return N2S(vtFld.lVal);
		}
	case VT_UI4://unsigned long
		{
			return N2S(vtFld.ulVal);
		}
	case VT_DECIMAL://decimal
		{
			if (vtFld.decVal.scale == 0)
			{
				return N2S(DecimalToLong(vtFld));
			}
			else
			{
				return N2S(DecimalToDouble(vtFld));
			}
		}
	case VT_DATE://日期
		{
			/*
			__time32_t tmm = 0;
			::VarI4FromDate(vtFld.date, &tmm);
			struct tm st; 
			_localtime32_s(&st, &tmm);

			char arr[24] = {0};
			sprintf_s(arr, 24, "%4d-%02d-%02d %02d:%02d:%02d", st.tm_year + 1970, 
				st.tm_mon + 1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec);*/

			VariantChangeType(&vtFld,&vtFld,0, VT_BSTR);
			//strftime();
			string str;
			str.assign(bstr_t(vtFld.bstrVal));
			return  str;
		}
		break;
	case VT_EMPTY:
	case VT_NULL://null
		{
			return "";
		}
	default:
		{
			return "";
		}
	}
}
