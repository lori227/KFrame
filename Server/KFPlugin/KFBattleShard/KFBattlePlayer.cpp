#include "KFBattlePlayer.h"
#include "KFBattleRoom.h"
#include "KFBattleConfig.h"
#include "KFOption/KFOptionInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

namespace KFrame
{
    KFBattlePlayer::KFBattlePlayer()
    {
        _notice_count = 0;
        _is_balance_score = false;
        _status = KFPlayerStatus::StatusPlayerIdle;
    }

    KFBattlePlayer::~KFBattlePlayer()
    {

    }

    KFBattlePlayer* KFBattlePlayer::Create( const KFMsg::PBBattlePlayer& pbplayer )
    {
        auto kfplayer = __KF_CREATE_BATCH__( KFBattlePlayer, 100 );
        kfplayer->LoadFrom( pbplayer );

        return kfplayer;
    }

    void KFBattlePlayer::Release( KFBattlePlayer* kfplayer )
    {
        __KF_DESTROY__( KFBattlePlayer, kfplayer );
    }

    uint32 KFBattlePlayer::GetID()
    {
        return _pb_player.playerid();
    }

    uint64 KFBattlePlayer::GetGroupID()
    {
        return _pb_player.groupid();
    }

    void KFBattlePlayer::LoadFrom( const KFMsg::PBBattlePlayer& pbplayer )
    {
        _pb_player.CopyFrom( pbplayer );

        auto temp = __TO_STRING__( KFUtility::Make64Guid( pbplayer.playerid() ) );
        _token = KFCrypto::Md5Encode( temp.c_str() );

        _status = KFPlayerStatus::StatusEnterRoom;
    }

    void KFBattlePlayer::SaveTo( KFMsg::PBBattlePlayer* pbplayer )
    {
        pbplayer->CopyFrom( _pb_player );
    }

    bool KFBattlePlayer::SendMessageToClient( uint32 msgid, google::protobuf::Message* message )
    {
        return _kf_cluster_shard->SendMessageToPlayer( _pb_player.serverid(), _pb_player.playerid(), msgid, message );
    }

    void KFBattlePlayer::ResetRoomStatus()
    {
        // 设置重新进入战场房间
        _status = KFPlayerStatus::StatusEnterRoom;
    }

    void KFBattlePlayer::RunEnterRoom( KFBattleRoom* kfroom )
    {
        switch ( _status )
        {
        case KFPlayerStatus::StatusEnterRoom:
        {
            // 玩家进去战场
            PlayerEnterBattleRoom( kfroom );
        }
        break;
        case KFPlayerStatus::StatusNoticeRoom:
        {
            // 通知玩家战场信息
            PlayerNoticeBattleRoom( kfroom );
        }
        break;
        default:
            break;
        }
    }

    void KFBattlePlayer::PlayerEnterBattleRoom( KFBattleRoom* kfroom )
    {
        KFMsg::S2SPlayerEnterBattleRoomReq req;
        SaveTo( req.mutable_player() );
        req.set_roomid( kfroom->_battle_room_id );
        req.set_token( _token );
        auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_ENTER_BATTLE_ROOM_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}] enter battle room[{}] req!", _pb_player.campid(), _pb_player.playerid(), kfroom->_battle_room_id );
        }
    }

    void KFBattlePlayer::ConfirmEnterBattleRoom()
    {
        _notice_count = 0;
        _status = KFPlayerStatus::StatusNoticeRoom;
    }

    void KFBattlePlayer::PlayerNoticeBattleRoom( KFBattleRoom* kfroom )
    {
        // 发送消息
        KFMsg::S2SNoticeMatchRoomReq req;
        req.set_matchid( kfroom->_match_id );
        req.set_battleshardid( KFGlobal::Instance()->_app_id );
        req.set_campid( _pb_player.campid() );
        req.set_playerid( _pb_player.playerid() );
        req.set_serverid( kfroom->_battle_server._server_id );
        req.set_ip( kfroom->_battle_server._ip );
        req.set_port( kfroom->_battle_server._port );
        req.set_roomid( kfroom->_battle_room_id );
        req.set_token( _token );
        auto ok = SendMessageToClient( KFMsg::S2S_NOTICE_MATCH_ROOM_REQ, &req );
        if ( ok )
        {
            ++_notice_count;
            if ( _notice_count >= 10 )
            {
                _status = KFPlayerStatus::StatusDisconnetRoom;
            }

            __LOG_DEBUG__( "player[{}:{}:{}] notice battle room[{}] req[{}]!",
                           _pb_player.campid(), KFAppID::ToString( _pb_player.serverid() ), _pb_player.playerid(),
                           kfroom->_battle_room_id, _notice_count );
        }
    }

    void KFBattlePlayer::ConfirmNoticeBattleRoom()
    {
        // 确认客户端已经收到战场信息
        _status = KFPlayerStatus::StatusLoginRoom;
    }

    bool KFBattlePlayer::LoginBattleRoomReq( KFBattleRoom* kfroom )
    {
        _status = KFPlayerStatus::StatusPlayingRoom;

        KFMsg::S2SPlayerLoginBattleRoomAck ack;
        ack.set_roomid( kfroom->_battle_room_id );
        ack.set_campid( _pb_player.campid() );
        ack.set_playerid( _pb_player.playerid() );
        auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_PLAYER_LOGIN_BATTLE_ROOM_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] login battle room[{}] failed!",
                           _pb_player.playerid(), kfroom->_battle_room_id );
        }

        return ok;
    }

    void KFBattlePlayer::LeaveBattleRoomReq( KFBattleRoom* kfroom )
    {
        if ( kfroom->IsBattleRoomStart() )
        {
            // 已经开始, 设置离开状态
            _status = KFPlayerStatus::StatusLeaveRoom;
        }
        else
        {
            // 发送消息到Match服务器
            KFMsg::S2SPlayerLeaveRoomToMatchShardReq req;
            req.set_matchid( kfroom->_match_id );
            req.set_roomid( kfroom->_battle_room_id );
            req.set_campid( _pb_player.campid() );
            req.set_playerid( _pb_player.playerid() );
            req.set_groupid( _pb_player.groupid() );
            kfroom->SendMessageToMatch( KFMsg::S2S_PLAYER_LEAVE_ROOM_TO_MATCH_SHARD_REQ, &req );
        }

        // 发送到游戏服务器
        SendLeaveRoomToGame( kfroom );
    }

    bool KFBattlePlayer::QueryBattleRoom( KFBattleRoom* kfroom, uint32 serverid )
    {
        // 设置游戏服务器id
        _pb_player.set_serverid( serverid );

        // 已经离开了 已经结算了
        if ( _status == KFPlayerStatus::StatusLeaveRoom || _status == KFPlayerStatus::StatusScoreBalance )
        {
            return false;
        }

        _notice_count = 0;
        PlayerNoticeBattleRoom( kfroom );
        return true;
    }

    void KFBattlePlayer::OnlineBattleRoom( uint32 serverid )
    {
        _pb_player.set_serverid( serverid );
        if ( _status == KFPlayerStatus::StatusDisconnetRoom )
        {
            _notice_count = 0;
            _status = KFPlayerStatus::StatusNoticeRoom;
            __LOG_DEBUG__( "player[{}] online ok!", _pb_player.playerid() );
        }
    }

    void KFBattlePlayer::FinishLeaveRoom( KFBattleRoom* kfroom )
    {
        SendLeaveRoomToGame( kfroom );
        _status = KFPlayerStatus::StatusLeaveRoom;
    }

    void KFBattlePlayer::SendLeaveRoomToGame( KFBattleRoom* kfroom )
    {
        KFMsg::S2SLeaveBattleRoomToClientAck ack;
        ack.set_playerid( GetID() );
        ack.set_roomid( kfroom->_battle_room_id );
        SendMessageToClient( KFMsg::S2S_LEAVE_BATTLE_ROOM_TO_CLIENT_ACK, &ack );
    }

    bool KFBattlePlayer::BattleScoreBalance( KFBattleRoom* kfroom, KFMsg::PBBattleScore* pbscore )
    {
        // 设置已经结算
        _status = KFPlayerStatus::StatusScoreBalance;

        // 设置matchid
        pbscore->set_matchid( kfroom->_match_id );
        pbscore->set_playercount( kfroom->_total_player_count );

        // 计算评分
        //ScoreBalance( pbscore );

        // 获得同阵营玩家
        GetSaveCampPlayer( kfroom, pbscore );

        // 获得奖励
        std::string reward = _invalid_str;
        if ( pbscore->has_score() )
        {
            auto kfsetting = _kf_battle_config->FindBattleReward( pbscore->matchid(), pbscore->score() );
            if ( kfsetting != nullptr )
            {
                reward = kfsetting->_reward;
                pbscore->set_reward( reward );
            }
        }

        {
            KFMsg::S2SBattleRoomScoreBalanceAck ack;
            ack.set_playerid( pbscore->playerid() );
            ack.set_roomid( kfroom->_battle_room_id );
            ack.set_reward( reward );
            auto ok = kfroom->SendMessageToBattle( KFMsg::S2S_BATTLE_ROOM_SCORE_BALANCE_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( "player[{}] balance score send failed!", _pb_player.playerid() );
            }
        }

        if ( !_is_balance_score )
        {
            _is_balance_score = true;

            // 发送到玩家
            KFMsg::S2SPlayerBattleScoreReq req;
            req.set_playerid( _pb_player.playerid() );
            req.set_roomid( kfroom->_battle_room_id );
            req.mutable_pbscore()->CopyFrom( *pbscore );
            SendMessageToClient( KFMsg::S2S_PLAYER_BATTLE_SCORE_REQ, &req );
        }

        return true;
    }

    void KFBattlePlayer::ScoreBalance( KFMsg::PBBattleScore* pbscore )
    {
        // 各项数据
        double battlescore = 0.0f;
        for ( auto i = 0; i < pbscore->pbdata_size(); ++i )
        {
            auto pbdata = &pbscore->pbdata( i );

            auto datacoefficient = _kf_option->GetDouble( __KF_STRING__( battlescoreparam ), pbdata->name() );
            battlescore += pbdata->value() * datacoefficient;
        }

        // 排名分
        {
            auto datacoefficient = _kf_option->GetUInt32( __KF_STRING__( battlescoreparam ), __KF_STRING__( ranking ) );
            battlescore += datacoefficient / pbscore->ranking();
        }

        // 吃鸡奖励分
        if ( pbscore->ranking() == __TOP_ONE__ )
        {
            auto datacoefficient = _kf_option->GetUInt32( __KF_STRING__( battlescoreparam ), __KF_STRING__( topone ) );
            battlescore += datacoefficient;
        }

        pbscore->set_score( __MAX__( 0.0f, battlescore ) );
    }

    void KFBattlePlayer::GetSaveCampPlayer( KFBattleRoom* kfroom, KFMsg::PBBattleScore* pbscore )
    {
        auto kfcamp = kfroom->_kf_camp_list.Find( _pb_player.campid() );
        if ( kfcamp == nullptr )
        {
            return;
        }

        for ( auto& iter : kfcamp->_kf_player_list._objects )
        {
            auto kfplayer = iter.second;
            if ( kfplayer->GetID() == GetID() )
            {
                continue;
            }

            pbscore->add_members( kfplayer->GetID() );
        }
    }
}