#include "KFKernelConfig.hpp"
#include "KFCore/KFDataConfig.h"

namespace KFrame
{
    bool KFKernelConfig::LoadConfig( const std::string& file )
    {
        return _kf_data_config->LoadDataConfig( file );
    }
}