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
        KFAuthModule();
        ~KFAuthModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 小区信息注册
        __KF_HTTP_FUNCTION__( HandleZoneRegister );

        // world注册
        __KF_HTTP_FUNCTION__( HandleWorldRegister );

        // 小区信息更新
        __KF_HTTP_FUNCTION__( HandleZoneUpdate );

        // 小区信息删除
        __KF_HTTP_FUNCTION__( HandleZoneRemove );

        // 查询小区列表
        __KF_HTTP_FUNCTION__( HandleQueryZoneList );

        // 查询小区ip
        __KF_HTTP_FUNCTION__( HandleQueryZoneIp );

        // 更新小区玩家注册数量
        __KF_HTTP_FUNCTION__( HandleZoneBalance );

        // 处理登录请求
        __KF_HTTP_FUNCTION__( HandleAuthLogin );

        // 验证token
        __KF_HTTP_FUNCTION__( HandleVerifyToken );

        // 申请订单
        __KF_HTTP_FUNCTION__( HandleApplyOrder );

        // 删除订单
        __KF_HTTP_FUNCTION__( HandleRemoveOrder );

        // 查询充值
        __KF_HTTP_FUNCTION__( HandleQueryPay );

        // 完成充值
        __KF_HTTP_FUNCTION__( HandleFinishPay );

        // 内部测试充值回调
        __KF_HTTP_FUNCTION__( HandleInternalPay );

    protected:
        // 查询创建账号
        MapString QueryCreateAccount( const std::string& account, uint32 channel );

        // 保存渠道数据
        void UpdateChannelData( uint64 accountid, uint32 channel, KFJson& kfjson );

        // 创建登录Token
        std::string SaveLoginToken( uint64 accountid, MapString& accountdata );

        // 踢人下线
        bool KickAccountOffline( uint64 playerid );

        // 获得小区信息
        std::string QueryZoneData( uint64 accountid, const std::string& token, MapString& accountdata );

        // 分配一个新小区
        uint32 BalanceAllocZoneId();

    private:
        // account锁
        KFMutex* _kf_mutex = nullptr;

        // accountid 生成器
        KFUUID* _account_make = nullptr;
    };
}



#endif