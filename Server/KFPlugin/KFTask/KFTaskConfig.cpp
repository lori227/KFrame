#include "KFTaskConfig.h"

namespace KFrame
{
    bool KFTaskConfig::LoadConfig()
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