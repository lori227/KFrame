#ifndef __KF_AUTH_MODULE_H__
#define __KF_AUTH_MODULE_H__

/************************************************************************
//    @Module			:    登录认证模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFAuthInterface.h"
#include "KFChannel/KFChannelInterface.h"
#include "KFAccount/KFAccountInterface.h"
#include "KFDirShard/KFDirShardInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFAuthModule : public KFAuthInterface
    {
    public:
        KFAuthModule() = default;
        ~KFAuthModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理登录请求
        __KF_HTTP_FUNCTION__( HandleAuthLogin );

        // 验证token
        __KF_HTTP_FUNCTION__( HandleVerifyToken );

        // 更新在线信息
        __KF_HTTP_FUNCTION__( HandleAccountOnline );

    private:
        // 踢掉在线玩家
        void KickAccount( uint32 type, uint64 accountid, StringMap& accountdata );
    };
}



#endif