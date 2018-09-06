#include "KFPublicShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    //#define __USE_WORKER__

#ifdef __USE_WORKER__
    #define __REGISTER_PUBLIC_MESSAGE__ __REGISTER_WORKER_MESSAGE__
    #define __UNREGISTER_PUBLIC_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
    #define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( public ) )
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
    #define __REGISTER_PUBLIC_MESSAGE__ __REGISTER_MESSAGE__
    #define __UNREGISTER_PUBLIC_MESSAGE__ __UNREGISTER_MESSAGE__
    static KFRedisDriver* _kf_redis_driver = nullptr;
    #define __PUBLIC_REDIS_DRIVER__ _kf_redis_driver
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

    KFPublicShardModule::KFPublicShardModule()
    {

    }

    KFPublicShardModule::~KFPublicShardModule()
    {

    }

    void KFPublicShardModule::InitModule()
    {
    }

    void KFPublicShardModule::BeforeRun()
    {
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ, &KFPublicShardModule::HandleCreateRoleReq );
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ, &KFPublicShardModule::HandleUpdatePublicDataReq );
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ, &KFPublicShardModule::HandleQueryBasicDataReq );
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ, &KFPublicShardModule::HandleSetPlayerNameReq );
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ, &KFPublicShardModule::HandleUpdateGuestListReq );
        __REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ, &KFPublicShardModule::HandleQueryGuestReq );
    }

    void KFPublicShardModule::BeforeShut()
    {
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ );
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ );
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ );
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ );
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ );
        __UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        _kf_schedule->UnRegisterSchedule( this );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFPublicShardModule::OnceRun()
    {
#ifndef __USE_WORKER__
        // 初始化redis
        _kf_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( public ) );
#endif
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdatePublicDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdatePublicDataReq );

        MapString values;
        for ( auto i = 0; i < kfmsg.pbdata_size(); ++i )
        {
            auto pbdata = &kfmsg.pbdata( i );
            values[ pbdata->name() ] = pbdata->value();
        }

        auto redisdriver = __PUBLIC_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( public ), kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryBasicDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryBasicReq );
        auto redisdriver = __PUBLIC_REDIS_DRIVER__;
        auto queryid = redisdriver->QueryUInt32( "get {}:{}", __KF_STRING__( name ), kfmsg.name() );

        KFMsg::S2SQueryBasicAck ack;
        ack.set_playerid( kfmsg.playerid() );

        auto pbobject = ack.mutable_pbobject();
        pbobject->set_key( queryid->_value );

        // 查询所有数据
        if ( queryid->_value != _invalid_int )
        {
            auto querydata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), queryid->_value );
            for ( auto& iter : querydata->_value )
            {
                auto pbstring = pbobject->add_pbstring();
                pbstring->set_name( iter.first );
                pbstring->set_value( iter.second );
            }
        }

        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_BASIC_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleCreateRoleReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateRoleReq );

        uint32 result = ProcessSetPlayerName( kfmsg.playerid(), _invalid_str, kfmsg.newname() );

        KFMsg::S2SCreateRoleAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_newname( kfmsg.newname() );
        ack.set_modleid( kfmsg.modleid() );
        ack.set_sex( kfmsg.sex() );
        ack.set_result( result );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_CREATE_ROLE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleSetPlayerNameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameReq );

        // 先查询名字
        uint32 result = ProcessSetPlayerName( kfmsg.playerid(), kfmsg.oldname(), kfmsg.newname() );

        KFMsg::S2SSetPlayerNameAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_oldname( kfmsg.oldname() );
        ack.set_newname( kfmsg.newname() );
        ack.set_result( result );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_SET_PLAYER_NAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdateGuestListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateGuestListReq );

        // 保存访客信息
        auto redisdriver = __PUBLIC_REDIS_DRIVER__;
        redisdriver->Execute( "zadd {}:{} {} {}",
                              __KF_STRING__( guest ), kfmsg.playerid(), kfmsg.guesttime(), kfmsg.guestid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryGuestReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryGuestReq );
        auto redisdriver = __PUBLIC_REDIS_DRIVER__;

        // 删除过期访客信息
        auto querytime = kfmsg.querytime();
        uint64 validtime = KFTimeEnum::OneDaySecond * KFTimeEnum::OneMonthDay;
        if ( querytime > validtime )
        {
            redisdriver->Execute( "zremrangebyscore {}:{} {} {}",
                                  __KF_STRING__( guest ), kfmsg.queryid(), _invalid_int, querytime - validtime );
        }

        // 返回访客信息
        KFMsg::S2SQueryGuestAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_queryid( kfmsg.queryid() );

        // 获取访客数量
        auto querycount = redisdriver->QueryUInt64( "zcard {}:{}", __KF_STRING__( guest ), kfmsg.queryid() );
        ack.set_guestcount( querycount->_value );

        // 获取最近四位访客的信息
        if ( querycount->_value != _invalid_int )
        {
            auto guestlist = redisdriver->QueryList( "zrevrange {}:{} 0 3", __KF_STRING__( guest ), kfmsg.queryid() );
            for ( auto& strid : guestlist->_value )
            {
                auto guestid = KFUtility::ToValue< uint32 >( strid );
                auto querydata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), guestid );

                auto pbguestdata = ack.add_guestdata();
                pbguestdata->set_guestid( guestid );
                pbguestdata->set_guesticon( querydata->_value[ __KF_STRING__( icon ) ] );
                pbguestdata->set_guesticonbox( querydata->_value[ __KF_STRING__( iconbox ) ] );
            }
        }

        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_GUEST_ACK, &ack );
    }

    uint32 KFPublicShardModule::ProcessSetPlayerName( uint32 playerid, const std::string& oldname, const std::string& newname )
    {
        auto redisdriver = __PUBLIC_REDIS_DRIVER__;

        auto queryplayerid = redisdriver->QueryUInt32( "get {}:{}", __KF_STRING__( name ), newname );
        if ( !queryplayerid->IsOk() )
        {
            return KFMsg::PublicDatabaseError;
        }

        // 如果已经设置, 并且不是自己, 返回错误
        if ( queryplayerid->_value != _invalid_int && queryplayerid->_value != playerid )
        {
            return KFMsg::NameAlreadyExist;
        }

        // 保存名字
        auto kfresult = redisdriver->Execute( "set {}:{} {}", __KF_STRING__( name ), newname, playerid );
        if ( !kfresult->IsOk() )
        {
            return KFMsg::PublicDatabaseError;
        }

        // 删除旧的名字关联
        if ( !oldname.empty() )
        {
            redisdriver->Execute( "del {}:{}", __KF_STRING__( name ), oldname );
        }

        return KFMsg::Success;
    }

}