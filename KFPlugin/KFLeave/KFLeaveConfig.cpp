#include "KFLeaveConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFLeaveSetting* KFLeaveConfig::CreateSetting( KFNode& xmlnode )
    {
        auto service = xmlnode.GetUInt32( "Service" );
        auto channel = xmlnode.GetUInt32( "Channel" );
        auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
        if ( !ok )
        {
            return nullptr;
        }

        return KFIntConfigT< KFLeaveSetting >::CreateSetting( xmlnode );
    }

    void KFLeaveConfig::ReadSetting( KFNode& xmlnode, KFLeaveSetting* kfsetting )
    {
        kfsetting->_lua_file = xmlnode.GetString( "LuaFile" );
        kfsetting->_lua_function = xmlnode.GetString( "LuaFunction" );
    }
}