#include "KFDataShardModule.h"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
#define __ZONE_REDIS_DRIVER__( zoneid ) _kf_redis->CreateExecute( __KF_STRING__( zone ), _invalid_int )

#define __REGISTER_DATA_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_DATA_MESSAGE__ __UNREGISTER_MESSAGE__
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendToClient( kfid, msgid, message )

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
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ );
        __UNREGISTER_DATA_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFDataShardModule::LoadPlayerData( uint32 zoneid, uint64 playerid, KFMsg::PBObject* pbobject )
    {
        // 先判断在keeper中是否存在
        auto kfkeeper = _kf_data_keeper.Find( playerid );
        if ( kfkeeper != nullptr )
        {
            pbobject->CopyFrom( kfkeeper->_pb_object );
            return true;
        }

        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis!", zoneid, playerid );
            return false;
        }

        auto kfresult = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( player ), playerid, __KF_STRING__( data ) );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "player[{}:{}] query failed!", zoneid, playerid );
            return false;
        }

        if ( !kfresult->_value.empty() )
        {
            auto ok = KFProto::Parse( pbobject, kfresult->_value, KFCompressEnum::Compress );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}:{}] parse failed!", zoneid, playerid );
                return false;
            }
        }

        return true;
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject )
    {
        auto strdata = KFProto::Serialize( pbobject, KFCompressEnum::Compress );
        if ( strdata == _invalid_str )
        {
            __LOG_ERROR__( "player[{}:{}] serialize failed!", zoneid, playerid );
            return false;
        }

        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            __LOG_ERROR__( "player[{}:{}] can't find redis!", zoneid, playerid );
            return false;
        }

        auto kfresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( player ), playerid, __KF_STRING__( data ), strdata );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "player[{}:{}] save failed!", zoneid, playerid );
            return false;
        }

        __LOG_DEBUG__( "player [{}:{}] save ok!", zoneid, playerid );
        return true;
    }

    __KF_TIMER_FUNCTION__( KFDataShardModule::OnTimerSaveDataKeeper )
    {
        std::set< uint64 > removes;
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

    void KFDataShardModule::DeletePlayerData( uint32 zoneid, uint64 playerid )
    {
#ifndef __KF_DEBUG__
        return;
#endif
        auto redisdriver = __ZONE_REDIS_DRIVER__( zoneid );
        if ( redisdriver == nullptr )
        {
            return;
        }

        redisdriver->Append( "del {}:{}", __KF_STRING__( player ), playerid );
        redisdriver->Append( "srem {}:{} {}", __KF_STRING__( playerlist ), zoneid, playerid );
        redisdriver->Pipeline();

        __LOG_DEBUG__( "player[{}:{}] delete ok!", zoneid, playerid );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginLoadPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] loaddata!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        KFMsg::S2SLoginLoadPlayerAck ack;
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        bool ok = LoadPlayerData( kfmsg.zoneid(), pblogin->playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Success );
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