#ifndef __KF_OPTION_CONFIG_H__
#define __KF_OPTION_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFPlatformConfig : public KFConfig, public KFSingleton< KFPlatformConfig >
    {
    public:
        KFPlatformConfig() = default;
        ~KFPlatformConfig() = default;

        // 加载配置
        bool LoadConfig();

    public:
        // appid
        std::string _app_id;

        // appkey
        std::string _app_key;

        // appurl
        std::string _app_url;

    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_platform_config = KFPlatformConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif