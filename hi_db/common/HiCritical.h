/**
*    @defgroup 通用模块
* @{
*/ 
#pragma once

#include <assert.h>

#if defined(_AFXEXT) || defined(_AFXDLL) || _HIS_USE_MFC
#else
#include <Windows.h>
#endif

/**
* @brief 临界区访问类，主要封装windows临界区的访问，该类主要在栈中使用，利用局部变量的构造和析构函数出入临界区
*	 @author  徐敏荣
*    @date    2012-06-14
*
* @par 修订历史
*    @version v0.5 \n
*	 @author  徐敏荣
*    @date    2012-06-14
*    @li 初始版本
*
*/
class HiCritical
{
public:

	/**
	*   @brief 构造函数
	*/
	HiCritical()
	{
		::InitializeCriticalSection(&cs);
	}

	/**
	*   @brief 析构函数
	*/
	~HiCritical()
	{
		::DeleteCriticalSection(&cs);
	}

	/**
	*   @brief 进入临界区
	*/
	void Enter()
	{
		::EnterCriticalSection(&cs);
	}
	
	/**
	*   @brief 离开临界区
	*/
	void Leave()
	{
		::LeaveCriticalSection(&cs);
	}

	CRITICAL_SECTION* GetSection()
	{
		return &cs;
	}
private:
	
	/**
	*   @brief 临界区对象
	*/
	CRITICAL_SECTION	cs;			/**< 临界区对象 */
};

/**
* @brief 临界区访问管理类，利用构造函数进入临界区，利用西沟函数离开临界区
*		如果向构造函数提供NULL参数，则不使用临界区。
*
*/
class HiCriticalMng
{
public:
	HiCriticalMng(HiCritical& crl): cl(&crl)
	{
		cl->Enter();
	}

	HiCriticalMng(HiCritical* crl): cl(crl)
	{
		if (cl)
		{
			cl->Enter();
		}
	}

	~HiCriticalMng()
	{
		if (cl)
		{
			cl->Leave();
		}
	}

private:
	HiCritical*   cl;
};

/**//** @}*/ // 通用模块