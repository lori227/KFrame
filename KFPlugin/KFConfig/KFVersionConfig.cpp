#include "KFVersionConfig.hpp"


namespace KFrame
{
    KFVersionSetting* KFVersionConfig::CreateSetting( KFNode& xmlnode )
    {
        auto id = xmlnode.GetString( "name" );
        auto kfsetting = _settings.Create( id );
        kfsetting->_id = id;
        return kfsetting;
    }

    void KFVersionConfig::ReadSetting( KFNode& xmlnode, KFVersionSetting* kfsetting )
    {
        kfsetting->_version = xmlnode.GetString( "version" );
    }
}