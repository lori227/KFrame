#include "KFProjectConfig.hpp"

namespace KFrame
{
    void KFProjectConfig::ReadSetting( KFNode& xmlnode, KFProjectSetting* kfsetting )
    {
        kfsetting->_str_value = xmlnode.GetString( "Value" );
        kfsetting->_uint64_value = KFUtility::ToValue<uint64>( kfsetting->_str_value );
        kfsetting->_double_value = KFUtility::ToValue<double>( kfsetting->_str_value );
    }

}