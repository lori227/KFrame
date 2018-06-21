#include "KFTaskConfig.h"

namespace KFrame
{
	bool KFTaskConfig::LoadConfig( const char* file )
	{
		_task_setting.Clear();

		try
		{
			
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}