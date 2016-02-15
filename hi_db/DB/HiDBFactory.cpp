#include "HiDBFactory.h"

#include "HiDBImpl.h"

#include "MySQL/HiDBMySQL.h"
#include "Oracle/HiDBOracle.h"

namespace NS_HiDB
{
	HiDBImpl* CreateDBImpl(HiDBType type, bool isUsingLock)
	{
		switch (type)
		{
		case HiDBType_MySQL:
		{
			return new HiDBMySQL(isUsingLock);
		}
		case HiDBType_Oracle:
		{
			return new HiDBOracle(isUsingLock);
		}
		default:
		{
			return new HiDBMySQL(isUsingLock);
		}
		}
	}
}