#include "KFDataShardModule.h"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
    static KFRedisDriver* _kf_account_redis = nullptr;
#define __ACCOUNT_REDIS_DRIVER__ _kf_account_redis
#define __ZONE_REDIS_DRIVER__( zoneid ) _kf_redis->CreateExecute( __KF_STRING__( zone ), zoneid )

#define __REGISTER_DATA_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_DATA_MESSAGE__ __UNREGISTER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )

    KFDataShardModule::KFDataShardModule()
    {

    }

    KFDataShardModule::~KFDataShardModule()
    {

    }

    void KFDataShardModule::InitModule()
    {
    }

    void KFDataShardModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 1, 10000, &KFDataShardModule::OnTimerSaveDataKeeper );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ, &KFDataShardModule::HandleSavePlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_DELETE_PLAYER_REQ, &KFDataShardModule::HandleDeletePlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_REQ, &KFDataShardModule::HandleLoginQueryPlayerIdReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &KFDataShardModule::HandleLoginLoadPlayerReq );
        __REGISTER_DATA_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ, &KFDataShardModule::HandleQueryPlayerReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDataShardModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_DELETE_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ );
    }

    void KFDataShardModule::OnceRun()
    {
        _kf_account_redis = _kf_redis->CreateExecute( __KF_STRING__( account ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFDataShardModule::QueryCreatePlayerId( uint32 channel, uint32 accountid, uint32 zoneid, uint32 logiczoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto uint32result = redisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                            __KF_STRING__( user ), accountid, logiczoneid );
        if ( !uint32result->IsOk() )
        {
            return _invalid_int;
        }

        // 存在playerid, 直接返回
        if ( uint32result->_value != _invalid_int )
        {
            return uint32result->_value;
        }

        // 创建playerid
        auto uint64result = redisdriver->QueryUInt64( __FUNC_LINE__, "incr {}:{}",
                            __KF_STRING__( playeridcreate ), zoneid );
        if ( !uint64result->IsOk() || uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        uint64 newuserid = uint64result->_value + 10000;
        auto playerid = KFUtility::CalcPlayerid( static_cast< uint32 >( newuserid ), zoneid );

        auto voidresult = redisdriver->Execute( __FUNC_LINE__, "hset {}:{} {} {}",
                                                __KF_STRING__( user ), accountid, logiczoneid, playerid );
        if ( !voidresult->IsOk() )
        {
            return _invalid_int;
        }

        return playerid;
    }

    void KFDataShardModule::CreatePlayer( uint32 channel, uint32 accountid, uint32 zoneid, uint32 playerid )
    {
        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid  );
        if ( redisdriver == nullptr )
        {
            return;
        }

        // 查询角色是否存在, 已经存在, 不需要创建
        auto kfresult = redisdriver->QueryUInt32( __FUNC_LINE__, "hget {}:{} {}",
                        __KF_STRING__( player ), playerid, __KF_STRING__( accountid ) );
        if ( kfresult->_value != _invalid_int )
        {
            return;
        }

        // 保存玩家信息
        MapString values;
        values[ __KF_STRING__( zoneid ) ] = KFUtility::ToString( zoneid );
        values[ __KF_STRING__( channel ) ] = KFUtility::ToString( channel );
        values[ __KF_STRING__( accountid ) ] = KFUtility::ToString( accountid );
        redisdriver->Append( values, "hmset {}:{}", __KF_STRING__( player ), playerid );
        redisdriver->Append( "sadd {}:{} {}", __KF_STRING__( playerlist ), zoneid, playerid );
        redisdriver->Pipeline( __FUNC_LINE__ );
    }

    bool KFDataShardModule::LoadPlayerData( uint32 zoneid, uint32 id, KFMsg::PBObject* pbobject )
    {
        // 先判断在keeper中是否存在
        auto kfkeeper = _kf_data_keeper.Find( id );
        if ( kfkeeper != nullptr )
        {
            pbobject->CopyFrom( kfkeeper->_pb_object );
            return true;
        }

        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] can't find redis!", zoneid, id );
            return false;
        }

        auto kfresult = redisdriver->QueryString( __FUNC_LINE__, "hget {}:{} {}",
                        __KF_STRING__( player ), id, __KF_STRING__( data ) );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] query failed!", zoneid, id );
            return false;
        }

        if ( !kfresult->_value.empty() )
        {
            auto ok = KFProto::Parse( pbobject, kfresult->_value, KFCompressEnum::Compress );
            if ( !ok )
            {
                __LOG_ERROR__( KFLogEnum::Login, "player[{}:{}] parse failed!", zoneid, id );
                return false;
            }
        }

        return true;
    }

    void KFDataShardModule::LoadExtendData( uint32 accountid, KFMsg::PBStrings* extenddata )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        auto kfresult = redisdriver->QueryMap( __FUNC_LINE__, "hgetall {}:{}", __KF_STRING__( extend ), accountid );
        for ( auto& iter : kfresult->_value )
        {
            auto pbstring = extenddata->add_pbstring();
            pbstring->set_name( iter.first );
            pbstring->set_value( iter.second );
        }
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint32 id, const KFMsg::PBObject* pbobject )
    {
        auto strdata = KFProto::Serialize( pbobject, KFCompressEnum::Compress );
        if ( strdata == _invalid_str )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}:{}] serialize failed!", zoneid, id );
            return false;
        }

        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}:{}] can't find redis!", zoneid, id );
            return false;
        }

        auto kfresult = redisdriver->Execute( __FUNC_LINE__, "hset {}:{} {} {}",
                                              __KF_STRING__( player ), id, __KF_STRING__( data ), strdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}:{}] save failed!", zoneid, id );
            return false;
        }

        __LOG_DEBUG__( KFLogEnum::Logic, "player [{}:{}] save ok!", zoneid, id );
        return true;
    }

    __KF_TIMER_FUNCTION__( KFDataShardModule::OnTimerSaveDataKeeper )
    {
        std::set< uint32 > removes;
        for ( auto iter : _kf_data_keeper._objects )
        {
            auto kfkeeper = iter.second;

            auto ok = SavePlayerData( kfkeeper->_zone_id, kfkeeper->_player_id, &kfkeeper->_pb_object );
            if ( ok )
            {
                removes.insert( iter.first );
            }
        }

        for ( auto playerid : removes )
        {
            _kf_data_keeper.Remove( playerid );
        }
    }

    void KFDataShardModule::DeletePlayerData( uint32 zoneid, uint32 id )
    {
#ifndef __KF_DEBUG__
        return;
#endif
        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            return;
        }

        redisdriver->Append( "del {}:{}", __KF_STRING__( player ), id );
        redisdriver->Append( "srem {}:{} {}", __KF_STRING__( playerlist ), zoneid, id );
        redisdriver->Pipeline( __FUNC_LINE__ );

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}:{}] delete ok!", zoneid, id );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginQueryPlayerIdReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryPlayerIdReq );

        auto channel = kfmsg.channel();
        auto accountid = kfmsg.accountid();
        auto zoneid = kfmsg.zoneid();
        auto logiczoneid = kfmsg.logiczoneid();

        auto playerid = QueryCreatePlayerId( channel, accountid, zoneid, logiczoneid );
        if ( playerid != 0 )
        {
            CreatePlayer( channel, accountid, zoneid, playerid );
        }

        KFMsg::S2SLoginQueryPlayerIdAck ack;
        ack.set_loginserverid( kfmsg.loginserverid() );
        ack.set_gateid( kfmsg.gateid() );
        ack.set_accountid( kfmsg.accountid() );
        ack.set_token( kfmsg.token() );
        ack.set_playerid( playerid );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_LOGIN_QUERY_PLAYERID_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginLoadPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerReq );

        KFMsg::S2SLoginLoadPlayerAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_gateid( kfmsg.gateid() );

        bool ok = LoadPlayerData( kfmsg.zoneid(), kfmsg.playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Success );
            LoadExtendData( kfmsg.accountid(), ack.mutable_extenddata() );
        }
        else
        {
            ack.set_result( KFMsg::LoadDataFailed );
        }
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSavePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSavePlayerReq );

        auto ok = SavePlayerData( kfmsg.zoneid(), kfmsg.id(), &kfmsg.data() );
        if ( ok )
        {
            _kf_data_keeper.Remove( kfmsg.id() );
        }
        else
        {
            // 保存失败 先缓存下来
            auto kfkeeper = _kf_data_keeper.Create( kfmsg.id() );
            kfkeeper->_player_id = kfmsg.id();
            kfkeeper->_zone_id = kfmsg.zoneid();
            kfkeeper->_pb_object.CopyFrom( kfmsg.data() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleDeletePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeletePlayerReq );

        DeletePlayerData( kfmsg.zoneid(), kfmsg.id() );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleQueryPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryPlayerReq );

        KFMsg::S2SQueryPlayerAck ack;
        ack.set_playerid( kfmsg.playerid() );

        LoadPlayerData( kfmsg.zoneid(), kfmsg.queryid(), ack.mutable_pbobject() );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_PLAYER_ACK, &ack );
    }

}