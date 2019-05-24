#ifndef __KF_CHANNEL_CONFIG_H__
#define __KF_CHANNEL_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFChannelSetting
    {
    public:
        KFChannelSetting()
        {
            _channel_id = 0;
            _debug_open = true;
            _release_open = false;
        }

        bool IsOpen() const;

    public:
        uint32 _channel_id;
        std::string _login_url;
        std::string _pay_url;
        std::string _app_id;
        std::string _app_key;
        bool _release_open;
        bool _debug_open;
    };

    class KFChannelConfig : public KFConfig, public KFSingleton< KFChannelConfig >
    {
    public:

        // 加载配置文件
        bool LoadConfig( const std::string& file );

        // 渠道是否开放
        bool IsChannelOpen( uint32 channel );

        // 查找渠道配置
        const KFChannelSetting* FindChannelSetting( uint32 channel );

    public:
        //////////////////////////////////////////////
        KFHashMap< uint32, uint32, KFChannelSetting > _kf_channel;

        // 渠道开放列表
        std::set< uint32 > _open_channel_list;
    };


    ////////////////////////////////////////////////////////////////////////////
    static auto _kf_channel_config = KFChannelConfig::Instance();
    ////////////////////////////////////////////////////////////////////////////

}


#endif
