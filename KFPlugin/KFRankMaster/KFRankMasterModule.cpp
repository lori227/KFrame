#include "KFRankMasterModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRankMasterModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_rank_config, true );
    }

    void KFRankMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFRankMasterModule::OnServerDisconverShard );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFRankMasterModule::OnServerLostShard );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REFRESH_RANK_ACK, &KFRankMasterModule::HandleRefreshRankAck );
    }

    void KFRankMasterModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_rank_config );

        __UNREGISTER_TIMER__();
        __UNREGISTER_SCHEDULE__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REFRESH_RANK_ACK );
    }

    void KFRankMasterModule::OnceRun()
    {
        // 启动刷新排行榜定时器
        StartRefreshRankDataTimer();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_NET_EVENT_FUNCTION__( KFRankMasterModule::OnServerDisconverShard )
    {
        if ( netdata->_name == __KF_STRING__( rank ) && netdata->_type == __KF_STRING__( shard ) )
        {
            _rank_shard_hash.AddHashNode( netdata->_name, netdata->_id, 100 );
        }
    }

    __KF_NET_EVENT_FUNCTION__( KFRankMasterModule::OnServerLostShard )
    {
        if ( netdata->_name == __KF_STRING__( rank ) && netdata->_type == __KF_STRING__( shard ) )
        {
            _rank_shard_hash.RemoveHashNode( netdata->_id );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////
    void KFRankMasterModule::StartRefreshRankDataTimer()
    {
        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            StartRefreshRankDataTimer( kfsetting );
        }
    }

    void KFRankMasterModule::StartRefreshRankDataTimer( const KFRankSetting* kfsetting )
    {
        if ( kfsetting->_refresh_type == KFTimeEnum::None )
        {
            // 配置_refresh_time 单位:秒
            __REGISTER_LOOP_TIMER__( kfsetting->_rank_id, kfsetting->_refresh_time * 1000, 0, &KFRankMasterModule::OnTimerRefreshRankData );
        }
        else
        {
            auto kfschedulesetting = _kf_schedule->CreateScheduleSetting();
            kfschedulesetting->SetData( kfsetting->_rank_id, nullptr, 0 );
            switch ( kfsetting->_refresh_type )
            {
            case KFTimeEnum::Week:
                kfschedulesetting->SetDayOfWeek( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
                break;
            default:
                kfschedulesetting->SetDate( KFScheduleEnum::Loop, kfsetting->_refresh_minute, kfsetting->_refresh_hour, kfsetting->_refresh_time );
                break;
            }

            __REGISTER_SCHEDULE__( kfschedulesetting, &KFRankMasterModule::OnScheduleRefreshRankData );
        }
    }

    __KF_TIMER_FUNCTION__( KFRankMasterModule::OnTimerRefreshRankData )
    {
        SendRefreshRankMessage( static_cast< uint32 >( objectid ) );
    }

    __KF_SCHEDULE_FUNCTION__( KFRankMasterModule::OnScheduleRefreshRankData )
    {
        SendRefreshRankMessage( static_cast< uint32 >( objectid ) );
    }

    void KFRankMasterModule::SendRefreshRankMessage( uint32 rankid )
    {
        auto shardid = _rank_shard_hash.FindHashNode( rankid, true );
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "rank=[{}] can't find shard!", rankid );
        }

        KFMsg::S2SRefreshRankReq req;
        req.set_rankid( rankid );
        auto ok = _kf_tcp_server->SendNetMessage( shardid, KFMsg::S2S_REFRESH_RANK_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "rank=[{}] send=[{}] failed!", rankid, KFAppId::ToString( shardid ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankMasterModule::HandleRefreshRankAck )
    {
        __PROTO_PARSE__( KFMsg::S2SRefreshRankAck );

        KFMsg::S2STellRefreshRank tell;
        tell.set_rankid( kfmsg.rankid() );
        _kf_tcp_server->SendMessageToType( __KF_STRING__( shard ), KFMsg::S2S_TELL_REFRESH_RANK, &tell );
    }
}