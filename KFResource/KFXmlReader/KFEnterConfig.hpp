#ifndef __KF_ENTER_CONFIG_H__
#define __KF_ENTER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFEnterSetting : public KFIntSetting
    {
    public:
        // lua脚本
        std::string _lua_file;

        // lua函数
        std::string _lua_function;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFEnterConfig : public KFConfigT< KFEnterSetting >, public KFInstance< KFEnterConfig >
    {
    public:
        KFEnterConfig()
        {
            _file_name = "enter";
        }

    protected:
        // 创建配置
        KFEnterSetting* CreateSetting( KFXmlNode& xmlnode )
        {
            auto service = xmlnode.ReadUInt32( "Service", true );
            auto channel = xmlnode.ReadUInt32( "Channel", true );
            auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
            if ( !ok )
            {
                return nullptr;
            }

            return KFConfigT< KFEnterSetting >::CreateSetting( xmlnode );
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFEnterSetting* kfsetting )
        {
            kfsetting->_lua_file = xmlnode.ReadString( "LuaFile" );
            kfsetting->_lua_function = xmlnode.ReadString( "LuaFunction" );
        }
    };
}

#endif