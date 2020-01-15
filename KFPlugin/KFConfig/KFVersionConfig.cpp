#include "KFVersionConfig.hpp"


namespace KFrame
{
    void KFVersionConfig::ReadSetting( KFNode& xmlnode, KFVersionSetting* kfsetting )
    {
        kfsetting->_old_version = kfsetting->_new_version;
        kfsetting->_new_version = xmlnode.GetString( "version" );
    }
}