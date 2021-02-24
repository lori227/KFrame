#ifndef __KF_ACCOUNT_REDIS_H__
#define __KF_ACCOUNT_REDIS_H__

#include "KFAccountLogic.hpp"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    class KFAccountRedis : public KFAccountLogic
    {
    public:
        KFAccountRedis() = default;
        ~KFAccountRedis() = default;

        // 查询账号数据
        virtual StringMap QueryCreateAccount( const std::string& account, uint32 channel );

        // 保存账号的渠道信息
        virtual void SaveChannelData( uint64 account_id,  KFJson& channel_data );

        // 查询账号的渠道信息
        virtual StringMap QueryChannelData( uint64 account_id );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建账号token
        virtual std::string MakeAccountToken( const std::string& account, uint32 channel, uint64 account_id, uint32 expire_time );

        // 验证账号token
        virtual StringMap VerifyAccountToken( uint64 account_id, const std::string& token );

        // 保存小区token, 默认保存一个月
        virtual void SaveZoneToken( const std::string& account, uint32 channel, uint64 account_id, uint32 zone_id, const std::string& token, uint32 expire_time );

        // 查询小区token
        virtual StringMap VerifyZoneToken( uint64 account_id, uint32 zone_id, const std::string& token );

        // 保存登录的ip和zoneid
        virtual void SaveLoginData( uint64 account_id, const std::string& ip, uint32 zone_id );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 创建账号角色id
        virtual std::tuple<uint64, bool> QueryCreatePlayer( uint64 account_id, uint32 zone_id );

        // 更新玩家在线信息
        virtual bool UpdateOnline( uint64 account_id, uint64 player_id, uint64 world_id, uint64 game_id );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 检查ip是否在黑名单
        virtual uint64 CheckIpInBlackList( const std::string& ip );

        // 添加ip黑名单
        virtual bool AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment );

        // 删除ip黑名单
        virtual bool RemoveIpBlackList( const std::string& ip );

        // 查询ip黑名单列表
        virtual StringMapList QueryIpBlackList();


        // 检查ip是否在白名单
        virtual bool CheckIpInWhiteList( const std::string& ip );

        // 添加ip白名单
        virtual bool AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment );

        // 删除ip白名单
        virtual bool RemoveIpWhiteList( const std::string& ip );

        // 查询ip白名单列表
        virtual StringMapList QueryIpWhiteList();


        // 检查账号是否在黑名单
        virtual uint64 CheckAccountInBlackList( uint64 account_id );

        // 添加账号黑名单
        virtual bool AddAccountBlackList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment );

        // 删除账号黑名单
        virtual bool RemoveAccountBlackList( uint64 account_id );

        // 查询账号黑名单列表
        virtual StringMapList QueryAccountBlackList();


        // 检查账号是否在白名单
        virtual bool CheckAccountInWhiteList( uint64 account_id );

        // 添加账号黑名单
        virtual bool AddAccountWhiteList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment );

        // 删除账号黑名单
        virtual bool RemoveAccountWhiteList( uint64 account_id );

        // 查询账号白名单列表
        virtual StringMapList QueryAccountWhiteList();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 保存微信token
        virtual void SaveWeiXinAccessToken( const std::string& machine_code, const std::string& openid,
                                            const std::string& scope, const std::string& access_token, uint32 expire_time );

        // 查询微信token
        virtual StringMap QueryWeiXinAccessToken( const std::string& machine_code );

        // 保存微信刷新token
        virtual void SaveWeiXinRefreshToken( const std::string& machine_code, const std::string& refresh_token );

        // 查询微信的刷新token
        virtual std::string QueryWeiXinRefreshToken( const std::string& machine_code );
    };
}



#endif