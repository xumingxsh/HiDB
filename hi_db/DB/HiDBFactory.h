#pragma once

#include "db/HiDBCommon.h"

class HiDBImpl;

namespace NS_HiDB
{
	HiDBImpl* CreateDBImpl(HiDBType type, bool isUsingLock);
}