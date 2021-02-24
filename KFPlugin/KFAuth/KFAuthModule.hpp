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
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFDirDatabase/KFDirDatabaseInterface.h"

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
        ////////////////////////////////////////////////////////////////////////////////
        // 添加ip黑名单
        __KF_HTTP_FUNCTION__( HandleAddIpBlackList );

        // 删除ip黑名单
        __KF_HTTP_FUNCTION__( HandleRemoveIpBlackList );

        // 查询ip黑名单
        __KF_HTTP_FUNCTION__( HandleQueryIpBlackList );

        // 添加ip白名单
        __KF_HTTP_FUNCTION__( HandleAddIpWhiteList );

        // 删除ip白名单
        __KF_HTTP_FUNCTION__( HandleRemoveIpWhiteList );

        // 查询ip白名单
        __KF_HTTP_FUNCTION__( HandleQueryIpWhiteList );

        // 添加账号黑名单
        __KF_HTTP_FUNCTION__( HandleAddAccountBlackList );

        // 删除账号黑名单
        __KF_HTTP_FUNCTION__( HandleRemoveAccountBlackList );

        // 查询账号黑名单
        __KF_HTTP_FUNCTION__( HandleQueryAccountBlackList );

        // 添加账号白名单
        __KF_HTTP_FUNCTION__( HandleAddAccountWhiteList );

        // 删除账号白名单
        __KF_HTTP_FUNCTION__( HandleRemoveAccountWhiteList );

        // 查询账号白名单
        __KF_HTTP_FUNCTION__( HandleQueryAccountWhiteList );

    private:
        // 踢掉在线玩家
        void KickAccount( uint32 type, uint64 account_id, StringMap& account_data );
    };
}



#endif