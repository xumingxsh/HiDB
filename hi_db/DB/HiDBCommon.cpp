#include "DB/HiDBCommon.h"

#include <sstream>

using namespace std;

HiDBException::HiDBException()
{
	m_errorId	= 0;
	m_dbTyp		= HiDBType_MySQL;
}

string HiDBException::to_string()
{
	ostringstream oss;
	oss<<"Info:HiDBException"
		<<";DBType:"<<m_dbTyp
		<<";Position:"<<m_position
		<<";SQL:"<<m_sql
		<<";Description:"<<m_descript
		<<";Error ID:"<<m_errorId;

	return oss.str();
}

const char* HiDBException::what() const
{
	HiDBException* ex = const_cast<HiDBException*>(this);
	ex->m_errFull = ex->to_string();
	return m_errFull.c_str();
}