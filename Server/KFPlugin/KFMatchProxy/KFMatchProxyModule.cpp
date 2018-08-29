#include "KFMatchProxyModule.h"

namespace KFrame
{
    KFMatchProxyModule::KFMatchProxyModule()
    {
    }

    KFMatchProxyModule::~KFMatchProxyModule()
    {
    }

    void KFMatchProxyModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFMatchProxyModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFMatchProxyModule::OnClientLostMatchShard );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_MATCH_REQ, &KFMatchProxyModule::HandleRegisterMatchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_PROXY_REQ, &KFMatchProxyModule::HandleMatchToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_ROOM_REQ, &KFMatchProxyModule::HandleQueryMatchRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_PROXY_REQ, &KFMatchProxyModule::HandleCancelMatchToProxyReq );
    }

    void KFMatchProxyModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_MATCH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_MATCH_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MATCH_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CANCEL_MATCH_TO_PROXY_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFMatchProxyModule::HandleRegisterMatchReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRegisterMatchReq );

        auto shardid = __KF_HEAD_ID__( kfguid );
        for ( auto i = 0; i < kfmsg.matchid_size(); ++i )
        {
            auto matchid = kfmsg.matchid( i );
            auto matchshard = _match_shard.Create( matchid );
            matchshard->AddHashNode( typeid( KFMatchProxyModule ).name(), shardid, 100 );

            __LOG_INFO__( KFLogEnum::Logic, "register match shard[{}:{}]!", shardid, matchid );
        }
    }

    __KF_CLIENT_LOST_FUNCTION__( KFMatchProxyModule::OnClientLostMatchShard )
    {
        for ( auto iter : _match_shard._objects )
        {
            auto matchshard = iter.second;

            auto ok = matchshard->RemoveHashNode( serverid );
            if ( ok )
            {
                __LOG_INFO__( KFLogEnum::Logic, "remove match shard[{}:{}]!", serverid, iter.first );
            }
        }
    }

    uint32 KFMatchProxyModule::FindMatchShard( uint32 matchid, uint32 playerid )
    {
        // 选择一个匹配服务器
        auto matchshard = _match_shard.Find( matchid );
        if ( matchshard == nullptr )
        {
            return _invalid_int;
        }

        return matchshard->FindHashNode( playerid, false );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchProxyModule::HandleMatchToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SMatchToProxyReq );
        auto clientid = __KF_HEAD_ID__( kfguid );

        auto shardid = _kf_cluster_proxy->FindObjectShard( kfmsg.matchid() );
        if ( shardid == _invalid_int )
        {
            KFMsg::S2SMatchToClientAck ack;
            ack.set_matchid( kfmsg.matchid() );
            ack.set_playerid( kfmsg.playerid() );
            ack.set_result( KFMsg::MatchCanNotFindServer );
            _kf_cluster_proxy->SendMessageToClient( clientid, KFMsg::S2S_MATCH_TO_CLIENT_ACK, &ack );

            return __LOG_ERROR__( KFLogEnum::Logic, "player[{}] can't find match[{}] shardid!", kfmsg.playerid(), kfmsg.matchid() );
        }

        // 发送消息
        KFMsg::S2SMatchToShardReq req;
        req.set_matchid( kfmsg.matchid() );
        req.set_playerid( kfmsg.playerid() );
        req.set_serverid( kfmsg.serverid() );
        req.set_allowgroup( kfmsg.allowgroup() );
        req.set_battleserverid( kfmsg.battleserverid() );
        req.mutable_pbgroup()->CopyFrom( kfmsg.pbgroup() );
        _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_MATCH_TO_SHARD_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMatchProxyModule::HandleQueryMatchRoomReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryMatchRoomReq );

        auto shardid = _kf_cluster_proxy->FindObjectShard( kfmsg.matchid() );
        if ( shardid == _invalid_int )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] can't find match[{}] shardid!", kfmsg.playerid(), kfmsg.matchid() );

            KFMsg::S2SQueryMatchRoomAck ack;
            ack.set_playerid( kfmsg.playerid() );
            ack.set_matchid( _invalid_int );
            _kf_cluster_proxy->SendMessageToClient( kfmsg.serverid(), KFMsg::S2S_QUERY_MATCH_ROOM_ACK, &ack );
        }
        else
        {
            KFMsg::S2SQueryRoomToMatchShardReq req;
            req.set_matchid( kfmsg.matchid() );
            req.set_playerid( kfmsg.playerid() );
            req.set_serverid( kfmsg.serverid() );
            _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_QUERY_ROOM_TO_MATCH_SHARD_REQ, &req );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMatchProxyModule::HandleCancelMatchToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCancelMatchToProxyReq );

        auto shardid = _kf_cluster_proxy->FindObjectShard( kfmsg.matchid() );
        if ( shardid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SCancelMatchToShardReq req;
        req.set_matchid( kfmsg.matchid() );
        req.set_playerid( kfmsg.playerid() );
        _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_CANCEL_MATCH_TO_SHARD_REQ, &req );
    }
}