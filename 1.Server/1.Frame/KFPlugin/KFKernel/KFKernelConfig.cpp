#include "KFKernelConfig.h"
#include "KFCore/KFDataConfig.h"

namespace KFrame
{
    bool KFKernelConfig::LoadConfig()
    {
        return _kf_data_config->LoadDataConfig( _file );
    }
}