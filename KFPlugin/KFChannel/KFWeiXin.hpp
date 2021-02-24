#ifndef __KF_WEI_XIN__H__
#define __KF_WEI_XIN__H__

#include "KFChannel.hpp"

namespace KFrame
{
    class KFWeiXin : public KFChannel
    {
    public:
        KFWeiXin() = default;
        virtual ~KFWeiXin() = default;

        // 登录请求
        std::string RequestLogin( KFJson& json, std::shared_ptr<const KFChannelSetting> setting );

        // 充值请求
        std::string RequestPay( const std::string& data, std::shared_ptr<const KFChannelSetting> setting );

    protected:
        // 保存访问token
        void SaveAccessToken( const std::string& machine_code, const std::string& open_id, const std::string& access_token, KFJson& json_data );
    };
}


#endif
