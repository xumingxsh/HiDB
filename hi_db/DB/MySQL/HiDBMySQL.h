﻿#pragma once
/**
*    @defgroup 数据库操作实现类接口类
*    @brief    数据库操作实现类接口类，声明数据库操作实现类的接口。
*	 @author  徐敏荣
*    @date    2012-06-14
*
* @par 修订历史
*    @version v0.5 \n
*	 @author  徐敏荣
*    @date    2012-06-14
*    @li 初始版本
* @{
*/ 
#include<winsock2.h>
#include <functional>
#include "mysql/mysql.h"

#include "../HiDBImpl.h"

/**
* @brief 数据库操作实现类接口类，声明数据库操作实现类的接口
*
*/
class  HiDBMySQL: public HiDBImpl
{
public:

	/**
	*   @brief 构造函数
	*	@param[in] isUsingLock	是否需要使用互斥锁
	*/
	HiDBMySQL(bool isUsingLock);

	/**
	*   @brief 析构函数
	*/
	~HiDBMySQL();

public:
		
	/**
	*   @brief 打开数据库连接
	*	@param[in] conn			数据库连接字符串
	*   @retval true：成功，false；失败
	*/
	bool Open(const char* conn);
		
	/**
	*   @brief 关闭据库连接
	*/
	void Close(void);

public:
		
	/**
	*   @brief 执行SQL语句，并不返回结果
	*	@param[in] conn			SQL语句
	*   @retval true：成功，false；失败
	*/
	bool ExecuteNoQuery(const char* sql);

public:

	/**
	*   @brief 执行SQL语句，返回一个结果
	*	@param[in] sql			SQL语句
	*	@param[out] value		取得的结果
	*   @retval true：成功，false；失败
	*/
	std::string ExecuteScalar(const char* sql);	

public:

	/**
	*   @brief 执行SQL语句，返回一个结果集合
	*	@param[in] sql			SQL语句
	*	@param[out] table		取得的结果集合
	*   @retval true：成功，false；失败
	*/
	std::shared_ptr<HiDBTable> ExecuteQuery(const char* sql);	

public:	
		
	/**
	*   @brief 事物处理
	*	@param[in] command	用户自定义命令
	*   @retval true：成功，false；失败
	*/
	void OnTransaction(const std::function<void()>& fun);		

private:
	bool IsOpen(void);

private:
	MYSQL*				m_pSQLData;
	MYSQL				m_SQLData;
	std::string				m_ConnString;

};

/**//** @}*/ // 数据库操作实现类接口类