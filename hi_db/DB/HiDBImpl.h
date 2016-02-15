
#pragma once
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

#include "DB/HiDB.h"
class HiCritical;
/**
* @brief 数据库操作实现类接口类，声明数据库操作实现类的接口
*
*/
class  HiDBImpl
{
public:

	/**
	*   @brief 构造函数
	*	@param[in] isUsingLock	是否需要使用互斥锁
	*/
	HiDBImpl(bool isUsingLock);

	/**
	*   @brief 析构函数
	*/
	virtual ~HiDBImpl();

public:
		
	/**
	*   @brief 打开数据库连接
	*	@param[in] conn			数据库连接字符串
	*   @retval true：成功，false；失败
	*/
	virtual bool Open(const char* conn) = 0;
		
	/**
	*   @brief 关闭据库连接
	*/
	virtual void Close(void) = 0;

public:
		
	/**
	*   @brief 执行SQL语句，并不返回结果
	*	@param[in] conn			SQL语句
	*   @retval true：成功，false；失败
	*/
	virtual bool ExecuteNoQuery(const char* sql) = 0;

public:

	/**
	*   @brief 执行SQL语句，返回一个结果
	*	@param[in] sql			SQL语句
	*	@param[out] value		取得的结果
	*   @retval true：成功，false；失败
	*/
	virtual std::string ExecuteScalar(const char* sql) = 0;
public:

	/**
	*   @brief 执行SQL语句，返回一个结果集合
	*	@param[in] sql			SQL语句
	*	@param[out] table		取得的结果集合
	*   @retval true：成功，false；失败
	*/
	virtual std::shared_ptr<HiDBTable> ExecuteQuery(const char* sql) = 0;

public:	
		
	/**
	*   @brief 事物处理
	*   @retval true：成功，false；失败
	*/
	virtual void OnTransaction(const std::function<void()>& fun) = 0;
	virtual bool IsOpen() = 0;
public:

	/**
	*   @brief 释放查询集合资源
	*	@param[in] table	查询的集合资源
	*   @retval true：成功，false；失败
	*/
	bool ReleaseHiDBTable(HiDBTable** table);

public:
	void SetLogFunction(const HiDBLogFun& fun);
protected:

	/**
	*   @brief 临界区对象，为空表示不需要考虑资源并发访问
	*/
	HiCritical*	m_pCritical;

public:
	HiDBLogFun logFun;
};

/**//** @}*/ // 数据库操作实现类接口类