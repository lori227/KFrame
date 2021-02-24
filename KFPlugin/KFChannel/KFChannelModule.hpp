#ifndef __KF_CHANNEL_MODULE_H__
#define __KF_CHANNEL_MODULE_H__

/************************************************************************
//    @Module			:    渠道接口（sdk）模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-3
************************************************************************/

#include "KFChannelInterface.h"
#include "KFConfig/KFChannelConfig.hpp"
#include "KFAccount/KFAccountInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFChannel;
    class KFChannelSetting;
    typedef std::function< std::string( KFJson&, std::shared_ptr<const KFChannelSetting> )> KFLoginFunction;
    typedef std::function< std::string( const std::string&, std::shared_ptr<const KFChannelSetting> )> KFPayFunction;

    class KFChannelModule : public KFChannelInterface
    {
    public:
        KFChannelModule() = default;
        ~KFChannelModule();

        // 初始化
        virtual void BeforeRun();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        template<class T>
        void RegisterChannel( uint32 channel, std::shared_ptr<T> object )
        {
            _kf_channel_list.Insert( channel, object );

            {
                KFLoginFunction login_function = std::bind( &T::RequestLogin, object, std::placeholders::_1, std::placeholders::_2 );
                auto function_data = _kf_login_function.Create( channel );
                function_data->SetFunction( login_function );
            }

            {
                KFPayFunction pay_function = std::bind( &T::RequestPay, object, std::placeholders::_1, std::placeholders::_2 );
                auto function_data = _kf_pay_function.Create( channel );
                function_data->SetFunction( pay_function );
            }
        }

        // 处理登录请求
        virtual std::string AuthLogin( KFJson& request );

        // 充值回调
        virtual std::string AuthPay( uint32 channel, const std::string& data );

    protected:
        // 渠道是否开放
        bool IsOpen( std::shared_ptr<const KFChannelSetting> setting ) const;

        // 渠道是否支持
        bool IsSupport( std::shared_ptr<const KFChannelSetting> setting, uint32 channel ) const;

    private:
        // 绑定的登录函数
        KFMapFunction<uint32, KFLoginFunction> _kf_login_function;

        // 充值函数
        KFMapFunction<uint32, KFPayFunction> _kf_pay_function;

        // 注册的渠道
        KFHashMap<uint32, KFChannel> _kf_channel_list;
    };
}



#endif