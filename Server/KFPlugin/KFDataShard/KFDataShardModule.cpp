#include "KFDataShardModule.h"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
#define __ZONE_REDIS_DRIVER__( zoneid ) _kf_redis->CreateExecute( __KF_STRING__( zone ), _invalid_int )

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

        auto kfresult = redisdriver->QueryString( "hget {}:{} {}",
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

        auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
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
        redisdriver->Pipeline();

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}:{}] delete ok!", zoneid, id );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoginLoadPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerReq );

        KFMsg::S2SLoginLoadPlayerAck ack;
        ack.set_channel( kfmsg.channel() );
        ack.set_gateid( kfmsg.gateid() );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_sessionid( kfmsg.sessionid() );
        ack.set_accountid( kfmsg.accountid() );
        ack.mutable_channeldata()->CopyFrom( kfmsg.channeldata() );
        bool ok = LoadPlayerData( kfmsg.zoneid(), kfmsg.playerid(), ack.mutable_playerdata() );
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