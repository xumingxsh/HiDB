#pragma once

/**
*    @defgroup 数据库模块
* @{
*/ 

#ifndef HI_DB_EXPORT
#ifdef HI_DB_DLL_API
#define HI_DB_EXPORT  __declspec(dllexport)
#else
#define HI_DB_EXPORT  __declspec(dllimport)
#endif	
#endif

#include <string>
#include <vector>
#include <map>
#include <sstream>
#pragma warning (disable: 4251)

/** @brief 数据库类型 */
enum HiDBType
{
	HiDBType_Invail,	/**<  无效类型 */
	HiDBType_MySQL,	/**<  MySQL */
	HiDBType_Oracle	/**<  Oracle */
};

#ifndef HiDBTable

typedef std::map<std::string, std::string> HiDBMap;

/** @brief 查询结果 */
typedef std::vector<HiDBMap> HiDBTable; /**<  查询结果 */
#endif

/** @brief 数据库操作异常 */
class HI_DB_EXPORT HiDBException: public std::exception
{
public:
	HiDBException();
public:
	const char* what() const;
public:
	std::string		m_sql;			/**<  本次操作的SQL语句 */
	std::string		m_descript;		/**<  异常描述 */
	std::string		m_position;		/**<  异常位置 */
	long			m_errorId;		/**<  异常编号 */
	HiDBType		m_dbTyp;		/**<  数据库类型 */
public:
	std::string to_string();
private:
	std::string m_errFull;
};

/**//** @}*/ // 数据库模块