#include "KFTimeConfig.hpp"

namespace KFrame
{
    // 读取配置
    void KFTimeConfig::ReadSetting( KFNode& xmlnode, KFTimeSetting* kfsetting )
    {
        kfsetting->_time_data._type = xmlnode.GetUInt32( "Type" );
        kfsetting->_time_data._value = xmlnode.GetUInt32( "Value", true );
        kfsetting->_time_data._hour = xmlnode.GetUInt32( "Hour", true );
    }
}
