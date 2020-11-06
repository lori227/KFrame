#ifndef __KF_PLAYER_CONFIG_H__
#define __KF_PLAYER_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
    namespace NewPlayerEnum
    {
        enum EConstEnum
        {
            NewAccount = 1,	// 新建账号
            NewRole = 2,	// 新建角色
        };
    }

    class KFPlayerSetting : public KFIntSetting
    {
    public:
        // 类型
        uint32 _type;

        // 奖励物品
        KFElements _elements;
    };
    ////////////////////////////////////////////////////////////////////////////////////
    class KFPlayerConfig : public KFConfigT< KFPlayerSetting >, public KFInstance< KFPlayerConfig >
    {
    public:
        KFPlayerConfig()
        {
            _file_name = "player";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;
                KFElementConfig::Instance()->ParseElement( kfsetting->_elements, __FILE__, __LINE__ );
            }
        }

    protected:
        // 创建配置
        KFPlayerSetting* CreateSetting( KFXmlNode& xmlnode )
        {
            auto service = xmlnode.ReadUInt32( "Service", true );
            auto channel = xmlnode.ReadUInt32( "Channel", true );
            auto ok = KFGlobal::Instance()->CheckChannelService( channel, service );
            if ( !ok )
            {
                return nullptr;
            }

            return KFConfigT< KFPlayerSetting >::CreateSetting( xmlnode );
        }

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFPlayerSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.ReadUInt32( "Type" );
            kfsetting->_elements._str_parse = xmlnode.ReadString( "Elements" );
        }
    };
}

#endif