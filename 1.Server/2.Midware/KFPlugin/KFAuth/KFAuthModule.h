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
        // 小区信息注册
        __KF_HTTP_FUNCTION__( HandleZoneRegister );

        // 小区信息更新
        __KF_HTTP_FUNCTION__( HandleZoneUpdate );

        // 查询小区列表
        __KF_HTTP_FUNCTION__( HandleQueryZoneList );

        // 查询小区ip
        __KF_HTTP_FUNCTION__( HandleQueryZoneIp );

        // 处理登录请求
        __KF_HTTP_FUNCTION__( HandleAuthLogin );

        // 激活请求
        __KF_HTTP_FUNCTION__( HandleAuthActivation );

        // 验证token
        __KF_HTTP_FUNCTION__( HandleVerifyToken );

        // 更新在线信息
        __KF_HTTP_FUNCTION__( HandleOnlineData );

        // ban人禁止游戏
        __KF_HTTP_FUNCTION__( HandleBanLogin );

        // 取消ban人
        __KF_HTTP_FUNCTION__( HandleUnBanLogin );

        // 查询ban人
        __KF_HTTP_FUNCTION__( HandleQueryBanLogin );

        // 查询玩家基本信息
        __KF_HTTP_FUNCTION__( HandleQueryAccountData );

    protected:
        // 查询创建账号
        MapString QueryCreateAccount( const std::string& account, uint32 channel );

        // 保存渠道数据
        void UpdateChannelData( uint64 accountid, uint32 channel, KFJson& kfjson );

        // 创建登录Token
        std::string CreateLoginToken( uint64 accountid, MapString& accountdata );

        // 踢人下线
        bool KickAccountOffline( MapString& accountdata );

        // 获得小区信息
        std::string QueryZoneData( uint64 accountid, const std::string& token, MapString& accountdata );

        // 分配一个新小区
        uint32 BalanceAllocZoneId();

        // 创建小区的玩家id
        uint64 QueryCreatePlayerId( uint32 channel, uint64 accountid, uint32 zoneid, uint32 logiczoneid );

        //验证激活码
        std::string VerifyActivationCode( MapString& accountdata, uint64 accountid, const std::string& activationcode );

        // 判断登陆黑名单
        uint64 CheckLoginBlackList( const std::string& ip, uint64 accountid, uint64 playerid );
    };
}



#endif