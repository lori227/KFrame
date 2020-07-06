#ifndef __KF_ACCOUNT_INTERFACE_H__
#define __KF_ACCOUNT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFAccountInterface : public KFModule
    {
    public:
        // 查询账号数据
        virtual StringMap QueryCreateAccount( const std::string& account, uint32 channel ) = 0;

        // 保存渠道的账号信息
        virtual void SaveChannelData( uint64 accountid, KFJson& channeldata ) = 0;

        // 查询账号的渠道信息
        virtual StringMap QueryChannelData( uint64 accountid ) = 0;

        // 创建账号token, 默认保存3600秒
        virtual std::string MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime = 3600u ) = 0;

        // 验证账号token
        virtual StringMap VerifyAccountToken( uint64 accountid, const std::string& token ) = 0;

        // 保存小区token, 默认保存一个月
        virtual void SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zoneid, const std::string& token, uint32 expiretime = 2592000u ) = 0;

        // 查询小区token
        virtual StringMap VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token ) = 0;

        // 保存登录的ip和zoneid
        virtual void SaveLoginData( uint64 accountid, const std::string& ip, uint32 zoneid ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建账号里的角色
        virtual std::tuple<uint64, bool> QueryCreatePlayer( uint64 accountid, uint32 zoneid ) = 0;

        // 更新玩家在线信息
        virtual bool UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid ) = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 检查ip是否在黑名单
        virtual uint64 CheckIpInBlackList( const std::string& ip ) = 0;

        // 添加ip黑名单
        virtual bool AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment ) = 0;

        // 删除ip黑名单
        virtual bool RemoveIpBlackList( const std::string& ip ) = 0;

        // 查询ip黑名单列表
        virtual StringMapList QueryIpBlackList() = 0;


        // 检查ip是否在白名单
        virtual bool CheckIpInWhiteList( const std::string& ip ) = 0;

        // 添加ip白名单
        virtual bool AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment ) = 0;

        // 删除ip白名单
        virtual bool RemoveIpWhiteList( const std::string& ip ) = 0;

        // 查询ip白名单列表
        virtual StringMapList QueryIpWhiteList() = 0;


        // 检查账号是否在黑名单
        virtual uint64 CheckAccountInBlackList( uint64 accountid ) = 0;

        // 添加ip黑名单
        virtual bool AddAccountBlackList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment ) = 0;

        // 删除ip黑名单
        virtual bool RemoveAccountBlackList( uint64 accountid ) = 0;

        // 查询账号黑名单列表
        virtual StringMapList QueryAccountBlackList() = 0;


        // 检查账号是否在白名单
        virtual bool CheckAccountInWhiteList( uint64 accountid ) = 0;

        // 添加ip黑名单
        virtual bool AddAccountWhiteList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment ) = 0;

        // 删除ip黑名单
        virtual bool RemoveAccountWhiteList( uint64 accountid ) = 0;

        // 查询账号白名单列表
        virtual StringMapList QueryAccountWhiteList() = 0;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 保存微信访问token
        virtual void SaveWeiXinAccessToken( const std::string& machinecode, const std::string& openid,
                                            const std::string& scope, const std::string& accesstoken, uint32 expirestime ) = 0;

        // 查询微信访问token
        virtual StringMap QueryWeiXinAccessToken( const std::string& machinecode ) = 0;

        // 保存微信刷新token
        virtual void SaveWeiXinRefreshToken( const std::string& machinecode, const std::string& refreshtoken ) = 0;

        // 查询微信的刷新token
        virtual std::string QueryWeiXinRefreshToken( const std::string& machinecode ) = 0;
    };
    ////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_account, KFAccountInterface );
    ////////////////////////////////////////////////////////////////////////

}


#endif