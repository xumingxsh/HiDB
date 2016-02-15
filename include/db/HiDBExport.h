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

#ifndef HISCOMMCALL
#define HISCOMMCALL __stdcall
#endif
/**//** @}*/ // 数据库模块