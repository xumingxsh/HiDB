/**
*    @defgroup 通用模块
* @{
*/ 
#pragma once

#include <functional>

class HisScopeGuard
{
public:
	explicit HisScopeGuard(std::function<void()> exitFun)
		: m_ExitFun(exitFun),m_dismissed_(false)
	{
	}
	~HisScopeGuard(void)
	{
		if (!m_dismissed_)
		{
			m_ExitFun();
		}
	}
	void dismissed()
	{
		m_dismissed_ = true;
	}
private:
	HisScopeGuard(const HisScopeGuard&);
	HisScopeGuard& operator=(const HisScopeGuard&);
private:
	std::function<void()>	m_ExitFun;
	bool				m_dismissed_;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) HisScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)
/**//** @}*/ // 通用模块