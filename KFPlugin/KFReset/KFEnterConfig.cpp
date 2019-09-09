#include "KFEnterConfig.hpp"

namespace KFrame
{
    KFEnterSetting* KFEnterConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true );
        auto channel = xmlnode.GetUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return KFConfigT< KFEnterSetting >::CreateSetting( xmlnode );
    }

    void KFEnterConfig::ReadSetting( KFNode& xmlnode, KFEnterSetting* kfsetting )
    {
        kfsetting->_lua_file = xmlnode.GetString( "LuaFile" );
        kfsetting->_lua_function = xmlnode.GetString( "LuaFunction" );
    }
}