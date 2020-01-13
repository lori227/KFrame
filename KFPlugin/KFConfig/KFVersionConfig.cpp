#include "KFVersionConfig.hpp"


namespace KFrame
{
    void KFVersionConfig::ReadSetting( KFNode& xmlnode, KFVersionSetting* kfsetting )
    {
        kfsetting->_version = xmlnode.GetString( "version" );
    }
}