#ifndef __KF_LEAVE_CONFIG_H__
#define __KF_LEAVE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFLeaveSetting : public KFIntSetting
    {
    public:
        // lua脚本
        std::string _lua_file;

        // lua函数
        std::string _lua_function;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFLeaveConfig : public KFConfigT< KFLeaveSetting >, public KFInstance< KFLeaveConfig >
    {
    public:
        KFLeaveConfig()
        {
            _file_name = "leave";
        }

    protected:
        // 创建配置
        KFLeaveSetting* CreateSetting( KFNode& xmlnode )
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

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFLeaveSetting* kfsetting )
        {
            kfsetting->_lua_file = xmlnode.GetString( "LuaFile" );
            kfsetting->_lua_function = xmlnode.GetString( "LuaFunction" );
        }
    };
}

#endif