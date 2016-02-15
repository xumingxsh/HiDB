#pragma once

/**
*    @defgroup 数据库模块
* @{
*/ 

#include <memory>
#include <functional>

#include "HiDBCommon.h"

class HiDBImpl;

#pragma warning (disable: 4290)

/// @enum	HiDBLogType
///
/// @brief	日志类型.
enum HiDBLogType
{
	HiDLT_Normal,		/// @brief	普通日志. 
	HiDLT_Exception,	/// @brief	异常日志. 
	HiDLT_SQL			/// @brief	SQL语句日志，主要调试时使用. 
};

/// @typedef	std::function<void(HiDBLogType,const char*)> HiDBLogFun
///
/// @brief	日志回调函数.
typedef std::function<void(HiDBLogType,const char*)> HiDBLogFun;

/**
* @brief 数据库操作类，封装数据库的通用操作，本类使用策略模式实现
*	 @author  徐敏荣
*    @date    2012-06-14
*
* @par 修订历史
*    @version v0.5 \n
*	 @author  徐敏荣
*    @date    2012-06-14
*    @li 初始版本
*    @version v0.6 \n
*	 @author  徐敏荣
*    @date    2014-08-04
*    @li 简化程序
*    @date    2014-08-04
*    @li 添加错误信息的报出，扩展异常为继承自std::exception
*    @date    2014-09-11
*    @li 修复多次close时崩溃的问题
*
*/
class HI_DB_EXPORT  HiDB
{
public:

	/**
	*   @brief 构造函数
	*	@param[in] type			数据库类型
	*	@param[in] isUsingLock	是否需要使用互斥锁
	*/
	HiDB(HiDBType type = HiDBType_MySQL, bool isUsingLock = false);

	/**
	*   @brief 析构函数
	*/
	~HiDB();

public:
	
	/**
	*   @brief 打开数据库连接
	*	@param[in] conn			数据库连接字符串
	*   @retval true：成功，false；失败
	*   @par 实例：
	*   @code MySQL
	*	HiDB db;
	*	if (db.Open("host=127.0.0.1;port=3306;dbname=test;user=root;pwd=root;charset=gbk;"))
	*	{
	*		// 打开成功
	*	}
	*	else
	*	{
	*		// 打开失败
	*	}
	*   @endcode
	*   @code  Oracle
	*	HiDB db;
	*	if (db.Open("Provider=OraOLEDB.Oracle.1;User ID=myName;Password=myPassword;"
	*		"Data Source=(DESCRIPTION=(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)"
	*		"(HOST=127.0.0.1)(PORT=1521)))(CONNECT_DATA=(SERVICE_NAME=testdb)));"))
	*	{
	*		// 打开成功
	*	}
	*	else
	*	{
	*		// 打开失败
	*	}
	*   @endcode
	*/
	bool Open(const char* conn) throw (HiDBException);
	
	/**
	*   @brief 关闭据库连接
	*/
	void Close(void);
	
	/**
	*   @brief 数据库连接是否打开
	*/
	bool IsOpen();

public:
	
	/**
	*   @brief 执行SQL语句，并不返回结果
	*	@param[in] conn			SQL语句
	*   @retval true：成功，false；失败
	*   @par 实例：
	*   @code
	*	HiDB db;
	*	if (db.ExecuteNoQuery("UPDATE table SET Paramer1='%s' 
	*		and Paramer2='%s' OR Paramer3=%d", "test1", "test2", 3))
	*	{
	*		// 执行成功
	*	}
	*	else
	*	{
	*		// 执行失败
	*	}
	*   @endcode
	*/
	bool ExecuteNoQuery(const char* sql, ...) throw (HiDBException);
	
public:

	/**
	*   @brief 执行SQL语句，返回一个结果
	*	@param[in] sql			SQL语句
	*   @retval 获得的数据，如果为空，则失败
	*/
	std::string ExecuteScalar(const char* sql, ...) throw (HiDBException);
	
public:

	/**
	*   @brief 执行SQL语句，返回一个结果集合
	*	@param[in] sql			SQL语句
	*   @retval 存储查询记录的智能指针
	*/
	std::shared_ptr<HiDBTable> ExecuteQuery(const char* sql, ...) throw (HiDBException);
	void ExecuteQuery(const std::function<void(const char*, const char*)>&,const char* sql, ...) throw (HiDBException);
	
public:	

	/**
	*   @brief 在事务中执行处理
	*	@param[in] fun	处理函数
	*/
	void OnTransaction(const std::function<void()>& fun) throw (HiDBException);

public:
	void SetLogFunction(const HiDBLogFun& fun);

private:
	/**
	*   @brief 数据库操作实现指针
	*/
	HiDBImpl*	m_Impl;		/**< 数据库操作实现指针 */
	HiDBLogFun	logFun;
};

/**//** @}*/ // 数据库模块