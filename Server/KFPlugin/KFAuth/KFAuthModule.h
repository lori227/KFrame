#ifndef __KF_AUTH_MODULE_H__
#define __KF_AUTH_MODULE_H__

/************************************************************************
//    @Module			:    登录认证模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFrame.h"
#include "KFAuthInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFChannel/KFChannelInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFPlatform/KFPlatformInterface.h"

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

        // 激活请求
        __KF_HTTP_FUNCTION__( HandleAuthActivation );

        // 验证token
        __KF_HTTP_FUNCTION__( HandleVerifyToken );

        // 更新在线信息
        __KF_HTTP_FUNCTION__( HandleUpdateOnline );

    protected:
        // 查询创建账号
        MapString QueryCreateAccount( const std::string& account, uint32 channel );

        // 保存渠道数据
        void UpdateChannelData( uint32 accountid, uint32 channel, KFJson& kfjson );

        // 创建登录Token
        std::string CreateLoginToken( uint32 accountid, MapString& accountdata );

        // 踢人下线
        bool KickAccountOffline( MapString& accountdata );

        // 获得目录服务器列表
        std::string QueryDirList( uint32 accountid, const std::string& token, MapString& accountdata );

        // 创建小区的玩家id
        uint32 QueryCreatePlayerId( uint32 channel, uint32 accountid, uint32 zoneid, uint32 logiczoneid );

        //验证激活码
        std::string VerifyActivationCode( KFResult<MapString>* accountdata, const uint32 accountid, const std::string& activationcode );
    };
}



#endif