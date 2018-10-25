#ifndef __KF_CHANNEL_MODULE_H__
#define __KF_CHANNEL_MODULE_H__

/************************************************************************
//    @Module			:    渠道接口（sdk）模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-3
************************************************************************/

#include "KFrame.h"
#include "KFChannelInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFChannel;
    class KFChannelSetting;
    class KFChannelModule : public KFChannelInterface
    {
    public:
        KFChannelModule();
        ~KFChannelModule();

        // 初始化
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        template< class T >
        void RegisterChannel( uint32 channel, T* object )
        {
            _kf_channel_list.insert( std::make_pair( channel, object ) );

            {
                KFChannelFunction loginfunction = std::bind( &T::RequestLogin, object, std::placeholders::_1, std::placeholders::_2 );
                auto kfloginfunction = _kf_login_function.Create( channel );
                kfloginfunction->_function = loginfunction;
            }

            {
                KFChannelFunction payfunction = std::bind( &T::RequestPay, object, std::placeholders::_1, std::placeholders::_2 );
                auto kfpayfunction = _kf_pay_function.Create( channel );
                kfpayfunction->_function = payfunction;
            }

        }

        // 处理登录请求
        virtual std::string AuthChannelLogin( const std::string& data );

    private:
        typedef std::function< std::string( KFJson& request, const KFChannelSetting* )> KFChannelFunction;

        // 绑定的登录函数
        KFBind< uint32, uint32, KFChannelFunction > _kf_login_function;

        // 充值函数
        KFBind< uint32, uint32, KFChannelFunction > _kf_pay_function;

        // 注册的渠道
        std::map< uint32, KFChannel* > _kf_channel_list;
    };
}



#endif