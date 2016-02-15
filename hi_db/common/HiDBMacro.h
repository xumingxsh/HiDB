#pragma once

#include "db/HiDBCommon.h"

/** @brief 异常语句宏 */
#define HiDBHelperOnError_void(ps, script,sql, id) \
	HiDBException exception;\
	exception.m_position = ps;\
	exception.m_descript = script;\
	exception.m_sql = sql;\
	exception.m_errorId = id;\
	logFun(HiDLT_Exception, exception.what());\
	throw exception;

/** @brief 异常语句宏 */
#define HiDBHelperOnError(ps, script,sql, id) \
	HiDBException exception;\
	exception.m_position = ps;\
	exception.m_descript = script;\
	exception.m_sql = sql;\
	exception.m_errorId = id;\
	logFun(HiDLT_Exception, exception.what());\
	throw exception;
//return false;