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
#include "KFChannelConfig.hpp"
#include "KFAccount/KFAccountInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFChannel;
    class KFChannelSetting;
    typedef std::function< std::string( KFJson&, const KFChannelSetting* )> KFLoginFunction;
    typedef std::function< std::string( const std::string&, const KFChannelSetting* )> KFPayFunction;

    class KFChannelModule : public KFChannelInterface
    {
    public:
        KFChannelModule() = default;
        ~KFChannelModule();

        // 初始化
        virtual void BeforeRun();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterChannel( uint32 channel, T* object )
        {
            _kf_channel_list.insert( std::make_pair( channel, object ) );

            {
                KFLoginFunction loginfunction = std::bind( &T::RequestLogin, object, std::placeholders::_1, std::placeholders::_2 );
                auto kfloginfunction = _kf_login_function.Create( channel );
                kfloginfunction->_function = loginfunction;
            }

            {
                KFPayFunction payfunction = std::bind( &T::RequestPay, object, std::placeholders::_1, std::placeholders::_2 );
                auto kfpayfunction = _kf_pay_function.Create( channel );
                kfpayfunction->_function = payfunction;
            }
        }

        // 处理登录请求
        virtual std::string AuthLogin( KFJson& request );

        // 充值回调
        virtual std::string AuthPay( uint32 channel, const std::string& data );
    private:
        // 绑定的登录函数
        KFBind< uint32, uint32, KFLoginFunction > _kf_login_function;

        // 充值函数
        KFBind< uint32, uint32, KFPayFunction > _kf_pay_function;

        // 注册的渠道
        std::unordered_map< uint32, KFChannel* > _kf_channel_list;
    };
}



#endif