#ifndef __KF_CHANNEL_CONFIG_H__
#define __KF_CHANNEL_CONFIG_H__

#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFChannelSetting : public KFIntSetting
    {
    public:
        // 是否开启
        bool IsOpen() const;

        // 是否支持
        bool IsSupport( uint32 channel ) const;
    public:
        // 登录验证地址
        std::string _login_url;

        // 充值验证地址
        std::string _pay_url;

        // appid
        std::string _app_id;

        // appkey
        std::string _app_key;

        // debug 是否开放
        bool _debug_open = true;

        // release 是否开放
        bool _release_open = true;

        // 额外开放的渠道
        std::set< uint32 > _support_list;
    };
    ////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////
    class KFChannelConfig : public KFIntConfigT< KFChannelSetting >, public KFSingleton< KFChannelConfig >
    {
    public:
        KFChannelConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFChannelSetting >( file, isclear )
        {
        }

        // 渠道是否开放
    protected:
        // 创建配置
        KFChannelSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFChannelSetting* kfsetting );
    };

    ////////////////////////////////////////////////////////////////////////////
    static auto _kf_channel_config = KFChannelConfig::Instance( "channel.xml", true );
    ////////////////////////////////////////////////////////////////////////////
}


#endif
