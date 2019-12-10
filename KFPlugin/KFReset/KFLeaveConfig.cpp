#include "KFLeaveConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFLeaveSetting* KFLeaveConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service", true );
        auto channel = xmlnode.GetUInt32( "Channel", true );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return KFConfigT< KFLeaveSetting >::CreateSetting( xmlnode );
    }

    void KFLeaveConfig::ReadSetting( KFNode& xmlnode, KFLeaveSetting* kfsetting )
    {
        kfsetting->_lua_file = xmlnode.GetString( "LuaFile" );
        kfsetting->_lua_function = xmlnode.GetString( "LuaFunction" );
    }
}