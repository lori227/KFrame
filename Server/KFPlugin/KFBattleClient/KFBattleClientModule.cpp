#include "KFBattleClientModule.h"

namespace KFrame
{
    KFBattleClientModule::KFBattleClientModule()
    {
        _kf_component = nullptr;
    }

    KFBattleClientModule::~KFBattleClientModule()
    {
    }

    void KFBattleClientModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFBattleClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterUpdateDataFunction( __KF_STRING__( player ), __KF_STRING__( roomid ), this, &KFBattleClientModule::OnRoomIdUpdateCallBack );

        _kf_player->RegisterEnterFunction( this, &KFBattleClientModule::OnEnterQueryBattleRoom );
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_NOTICE_MATCH_ROOM_REQ, &KFBattleClientModule::HandleNoticeMatchRoomReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LEAVE_BATTLE_ROOM_TO_CLIENT_ACK, &KFBattleClientModule::HandleLeaveBattleRoomAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_BATTLE_SCORE_REQ, &KFBattleClientModule::HandlePlayerBattleScoreReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_ACK, &KFBattleClientModule::HandleQueryBattleRoomAck );
    }

    void KFBattleClientModule::BeforeShut()
    {
        _kf_player->UnRegisterEnterFunction( this );
        _kf_component->UnRegisterUpdateDataFunction( this, __KF_STRING__( player ), __KF_STRING__( roomid ) );
        ////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NOTICE_MATCH_ROOM_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LEAVE_BATTLE_ROOM_TO_CLIENT_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_BATTLE_SCORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_BATTLE_ROOM_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFBattleClientModule::SendMessageToBattle( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToShard( __KF_STRING__( battle ), msgid, message );
    }

    bool KFBattleClientModule::SendMessageToBattle( uint64 roomid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendMessageToObject( __KF_STRING__( battle ), roomid, msgid, message );
    }

    void KFBattleClientModule::OnEnterQueryBattleRoom( KFEntity* player )
    {
        {
            // 上线查询结算信息
            KFMsg::S2SOnlieQueryBattleScoreReq req;
            req.set_playerid( player->GetKeyID() );
            SendMessageToBattle( KFMsg::S2S_ONLINE_QUERY_BATTLE_SCORE_REQ, &req );
        }

        auto kfobject = player->GetData();
        auto roomid = kfobject->GetValue< uint64 >( __KF_STRING__( roomid ) );
        if ( roomid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SQueryBattleRoomReq req;
        req.set_roomid( roomid );
        req.set_playerid( player->GetKeyID() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        SendMessageToBattle( KFMsg::S2S_QUERY_BATTLE_ROOM_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleClientModule::HandleQueryBattleRoomAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryBattleRoomAck );

        player->UpdateData( __KF_STRING__( matchid ), KFOperateEnum::Set, kfmsg.matchid() );
        player->UpdateData( __KF_STRING__( roomid ), KFOperateEnum::Set, kfmsg.roomid() );

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] query battle room[{}:{}]!",
                       player->GetKeyID(), kfmsg.matchid(), kfmsg.roomid() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleClientModule::HandleNoticeMatchRoomReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SNoticeMatchRoomReq );

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] match[{}] room[{}|{}:{}] req!",
                       player->GetKeyID(), kfmsg.matchid(), kfmsg.roomid(), kfmsg.ip(), kfmsg.port() );

        auto kfobject = player->GetData();
        auto matchid = kfobject->GetValue< uint32 >( __KF_STRING__( matchid ) );
        if ( matchid == _invalid_int || matchid != kfmsg.matchid() )
        {
            return __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] match[{}] room[{}|{}:{}] cancel!",
                                  player->GetKeyID(), kfmsg.matchid(), kfmsg.roomid(), kfmsg.ip(), kfmsg.port() );
        }

        player->UpdateData( __KF_STRING__( roomid ), KFOperateEnum::Set, kfmsg.roomid() );
        {
            // 发送到客户端
            KFMsg::MsgMatchResultAck ack;
            ack.set_matchid( kfmsg.matchid() );
            ack.set_roomid( kfmsg.roomid() );
            ack.set_serverid( kfmsg.serverid() );
            ack.set_ip( kfmsg.ip() );
            ack.set_port( kfmsg.port() );
            ack.set_token( kfmsg.token() );
            auto ok = _kf_player->SendMessageToClient( player, KFMsg::MSG_MATCH_RESULT_ACK, &ack );
            if ( !ok )
            {
                __LOG_ERROR__( KFLogEnum::Logic, "player[{}] match[{}] room[{}|{}:{}] failed!",
                               player->GetKeyID(), kfmsg.matchid(), kfmsg.roomid(), kfmsg.ip(), kfmsg.port() );
            }
        }

        {
            // 更新到战斗服务器集群
            KFMsg::S2SNoticeMatchRoomAck ack;
            ack.set_roomid( kfmsg.roomid() );
            ack.set_campid( kfmsg.campid() );
            ack.set_playerid( kfmsg.playerid() );
            SendMessageToBattle( kfmsg.roomid(), KFMsg::S2S_NOTICE_MATCH_ROOM_ACK, &ack );
        }

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] match[{}] room[{}|{}:{}] ok!",
                       player->GetKeyID(), kfmsg.matchid(), kfmsg.roomid(), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFBattleClientModule::HandleLeaveBattleRoomAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLeaveBattleRoomToClientAck );

        // 取消配置信息
        player->UpdateData( __KF_STRING__( roomid ), KFOperateEnum::Set, _invalid_int );
        player->UpdateData( __KF_STRING__( matchid ), KFOperateEnum::Set, _invalid_int );

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] leave battle room!", player->GetKeyID() );
    }

    void KFBattleClientModule::BalanceBattleScore( KFEntity* player, KFData* kfscore, const KFMsg::PBBattleScore* pbscore )
    {
        // 游戏场数
        auto kfobject = player->GetData();
        auto totalcount = player->UpdateData( kfscore, __KF_STRING__( count ), KFOperateEnum::Add, 1 );

        // 排名
        if ( pbscore->ranking() == __TOP_ONE__ )
        {
            player->UpdateData( kfscore, __KF_STRING__( victory ), KFOperateEnum::Add, 1 );
        }
        else if ( pbscore->ranking() <= __TOP_FIVE__ )
        {
            player->UpdateData( kfscore, __KF_STRING__( topfive ), KFOperateEnum::Add, 1 );
        }
        else if ( pbscore->ranking() <= __TOP_TEN__ )
        {
            player->UpdateData( kfscore, __KF_STRING__( topten ), KFOperateEnum::Add, 1 );
        }

        // 各项数据
        for ( auto i = 0; i < pbscore->pbdata_size(); ++i )
        {
            auto pbdata = &pbscore->pbdata( i );

            // 更新数值
            player->UpdateData( kfscore, pbdata->name(), KFOperateEnum::Add, pbdata->value() );

            // 更新最大数值
            std::string maxname = "max" + pbdata->name();
            player->UpdateData( kfscore, maxname, KFOperateEnum::Greater, pbdata->value() );
        }

        // 胜率
        auto victorycount = kfscore->GetValue< uint32 >( __KF_STRING__( victory ) );
        auto winrate = static_cast< double >( victorycount ) / static_cast< double >( totalcount );
        player->UpdateData( kfscore, __KF_STRING__( winrate ), KFOperateEnum::Set, winrate * KFRatioEnum::Ratio );

        // kda计算
        auto killcount = kfscore->GetValue< uint32 >( __KF_STRING__( kill ) );
        auto diecount = kfscore->GetValue< uint32 >( __KF_STRING__( die ) );
        auto kda = static_cast< double >( killcount ) / static_cast< double >( __MAX__( 1, diecount ) );
        player->UpdateData( kfscore, __KF_STRING__( kda ), KFOperateEnum::Set, kda * KFRatioEnum::Ratio );

        // 场均击杀
        auto averagekill = static_cast< double >( killcount ) / static_cast< double >( totalcount );
        player->UpdateData( kfscore, __KF_STRING__( averagekill ), KFOperateEnum::Set, averagekill * KFRatioEnum::Ratio );

        // 奖励
        if ( pbscore->has_reward() )
        {
            KFAgents kfagents;
            kfagents.ParseFromString( pbscore->reward(), __FUNC_LINE__ );
            player->AddAgentData( &kfagents, 1.0f, false, __FUNC_LINE__ );
        }

        // 成就
        if ( pbscore->has_achieve() )
        {
            auto kfachieves = kfobject->FindData( __KF_STRING__( achieve ) );
            auto pbachieves = &pbscore->achieve();
            for ( auto i = 0; i < pbachieves->taskdata_size(); ++i )
            {
                auto pbachieve = &pbachieves->taskdata( i );
                player->UpdateData( kfachieves, pbachieve->id(), __KF_STRING__( value ), KFOperateEnum::Set, pbachieve->value() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFBattleClientModule::HandlePlayerBattleScoreReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SPlayerBattleScoreReq );

        auto pbscore = &kfmsg.pbscore();
        auto kfobject = player->GetData();
        auto scorename = _kf_option->GetValue< std::string >( __KF_STRING__( matchscore ), pbscore->matchid() );

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] balance battle[{}] score[{}:{}] req!",
                       kfmsg.playerid(), kfmsg.roomid(), pbscore->matchid(), scorename );

        auto kfscore = kfobject->FindData( scorename );
        if ( kfscore == nullptr )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] balance battle[{}] score[{}:{}] error!",
                           kfmsg.playerid(), kfmsg.roomid(), pbscore->matchid(), scorename );
            return;
        }

        // 计算单项数据
        BalanceBattleScore( player, kfscore, pbscore );
        int32 score = pbscore->score();
        if ( score > 0 )
        {
            player->UpdateData( kfscore, __KF_STRING__( score ), KFOperateEnum::Add, score );
        }
        else
        {
            player->UpdateData( kfscore, __KF_STRING__( score ), KFOperateEnum::Dec, abs( score ) );
        }

        // 计算总数据
        auto kftotalscore = kfobject->FindData( __KF_STRING__( totalscore ) );
        if ( kftotalscore != nullptr )
        {
            BalanceBattleScore( player, kftotalscore, pbscore );

            // 总评分
            auto totalscore = CalcTotalScore( player );
            player->UpdateData( kftotalscore, __KF_STRING__( score ), KFOperateEnum::Set, totalscore );
        }

        // 结算关系属性
        _kf_relation->BalanceBattleRelation( player, kfmsg.roomid(), pbscore );

        // 回复消息
        KFMsg::S2SPlayerBattleScoreAck ack;
        ack.set_roomid( kfmsg.roomid() );
        ack.set_playerid( kfmsg.playerid() );
        auto ok = SendMessageToBattle( KFMsg::S2S_PLAYER_BATTLE_SCORE_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] balance battle[{}] score[{}:{}] ok!",
                           kfmsg.playerid(), kfmsg.roomid(), pbscore->matchid(), scorename );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] balance battle[{}] score[{}:{}] failed!",
                           kfmsg.playerid(), kfmsg.roomid(), pbscore->matchid(), scorename );
        }
    }

    uint32 KFBattleClientModule::CalcTotalScore( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto singlescore = kfobject->FindData( __KF_STRING__( singlescore ) );
        auto doublescore = kfobject->FindData( __KF_STRING__( doublescore ) );
        auto fourscore = kfobject->FindData( __KF_STRING__( fourscore ) );

        auto totalgametimes = singlescore->GetValue<uint32>( __KF_STRING__( count ) )
                              + doublescore->GetValue<uint32>( __KF_STRING__( count ) )
                              + fourscore->GetValue<uint32>( __KF_STRING__( count ) );

        if ( totalgametimes == _invalid_int )
        {
            return _invalid_int;
        }

        auto tempvalue = singlescore->GetValue( __KF_STRING__( score ) ) * singlescore->GetValue( __KF_STRING__( count ) )
                         + doublescore->GetValue( __KF_STRING__( score ) ) * doublescore->GetValue( __KF_STRING__( count ) )
                         + fourscore->GetValue( __KF_STRING__( score ) ) * fourscore->GetValue( __KF_STRING__( count ) );

        return tempvalue / totalgametimes;
    }

    __KF_UPDATE_DATA_FUNCTION__( KFBattleClientModule::OnRoomIdUpdateCallBack )
    {
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        if ( newvalue != _invalid_int )
        {
            player->UpdateData( kfbasic, __KF_STRING__( status ), KFOperateEnum::Set, KFMsg::PlayingStatus );
        }
    }

}