#include "KFRankShardModule.h"
#include "KFTime/KFDate.h"
#include "KFCompress/KFCompress.h"

namespace KFrame
{
    KFRankShardModule::KFRankShardModule()
    {
        _rank_driver = nullptr;
    }

    KFRankShardModule::~KFRankShardModule()
    {

    }

    void KFRankShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_rank_config, true );
    }

    void KFRankShardModule::BeforeRun()
    {
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFRankShardModule::OnServerDiscoverClient );
        //////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ, &KFRankShardModule::HandleUpdateRankDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ, &KFRankShardModule::HandleQueryRanklistReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &KFRankShardModule::HandleQueryFriendRanklistReq );
    }

    void KFRankShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        //////////////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_RANK_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_RANK_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ );
    }

    void KFRankShardModule::OnceRun()
    {
        _rank_driver = _kf_redis->CreateExecute( KFField::_rank );

        // 加载所有排行榜数据
        LoadTotalRankData();

        // 启动刷新排行榜定时器
        StartRefreshRankDataTimer();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFRankShardModule::OnServerDiscoverClient )
    {
        std::list< uint64 > ranklist;

        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_shard_id == KFGlobal::Instance()->_app_id )
            {
                ranklist.push_back( kfsetting->_rank_id );
            }
        }

        _kf_cluster_shard->AddObjectToProxy( handleid, ranklist );
    }

    void KFRankShardModule::LoadTotalRankData()
    {
        VectorString ranklist;
        _rank_driver->VectorExecute( ranklist, "smembers %s:%u", KFField::_rank_list.c_str(), KFGlobal::Instance()->_app_id );
        if ( ranklist.empty() )
        {
            return;
        }

        for ( auto& strrankkey : ranklist )
        {
            MapString mapvalues;
            _rank_driver->MapExecute( mapvalues, "hgetall %s", strrankkey.c_str() );
            if ( mapvalues.empty() )
            {
                continue;
            }

            auto rankid = KFUtility::ToValue< uint32 >( mapvalues[ KFField::_id ] );
            auto zoneid = KFUtility::ToValue< uint32 >( mapvalues[ KFField::_zone_id ] );
            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = KFUtility::ToValue< uint64 >( mapvalues[ KFField::_min_rank_score ] );

            auto strrankata = mapvalues[ KFField::_rank_data ];
            KFProto::Parse( &kfrankdata->_rank_datas, strrankata, KFCompressEnum::Compress );
        }

        _rank_driver->VoidExecute( "del %s:%u", KFField::_rank_list.c_str(), KFGlobal::Instance()->_app_id );
    }

    void KFRankShardModule::SaveRankData( KFRankData* kfrankdata )
    {
        std::string strdata = "";
        auto strrankdata = KFProto::Serialize( &kfrankdata->_rank_datas, KFCompressEnum::Compress );

        MapString rankdata;
        rankdata[ KFField::_id ] = __KF_STRING__( kfrankdata->_rank_id );
        rankdata[ KFField::_zone_id ] = __KF_STRING__( kfrankdata->_zone_id );
        rankdata[ KFField::_min_rank_score ] = __KF_STRING__( kfrankdata->_min_rank_score );
        rankdata[ KFField::_rank_data ] = strrankdata;

        auto rankkey = KFUtility::Format( "%s:%u:%u", KFField::_rank.c_str(), kfrankdata->_rank_id, kfrankdata->_zone_id );

        _rank_driver->AppendCommand( rankdata, "hmset %s", rankkey );
        _rank_driver->AppendCommand( "sadd %s:%u %s", KFField::_rank_list.c_str(), KFGlobal::Instance()->_app_id, rankkey );
        _rank_driver->PipelineExecute();
    }

    void KFRankShardModule::StartRefreshRankDataTimer()
    {
        for ( auto& iter : _kf_rank_config->_kf_rank_setting._objects )
        {
            auto kfsetting = iter.second;
            if ( kfsetting->_shard_id != KFGlobal::Instance()->_app_id )
            {
                continue;
            }

            StartRefreshRankDataTimer( kfsetting );
        }
    }

    void KFRankShardModule::StartRefreshRankDataTimer( const KFRankSetting* kfsetting )
    {
        if ( kfsetting->_refresh_type == KFTimeEnum::None )
        {
            // 配置_refresh_time 单位:秒
            __REGISTER_LOOP_TIMER__( kfsetting->_rank_id, kfsetting->_refresh_time * 1000, &KFRankShardModule::OnTimerRefreshRankData );
            return;
        }

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

        _kf_schedule->RegisterSchedule( kfschedulesetting, this, &KFRankShardModule::OnScheduleRefreshRankData );
    }

    __KF_TIMER_FUNCTION__( KFRankShardModule::OnTimerRefreshRankData )
    {
        RefreshRankData( objectid );
    }

    void KFRankShardModule::OnScheduleRefreshRankData( uint32 rankid, const char* data, uint32 size )
    {
        RefreshRankData( rankid );
    }

    std::string& KFRankShardModule::FormatRankDataKey( uint32 rankid, uint32 zoneid )
    {
        static std::string rankdatakey = "";
        rankdatakey = KFUtility::Format( "%s:%u:%u", KFField::_rank_data.c_str(), rankid, zoneid );
        return rankdatakey;
    }

    std::string& KFRankShardModule::FormatRankSortKey( uint32 rankid, uint32 zoneid )
    {
        static std::string ranksortkey = "";
        ranksortkey = KFUtility::Format( "%s:%u:%u", KFField::_rank_sort.c_str(), rankid, zoneid );
        return ranksortkey;
    }

    void KFRankShardModule::RefreshRankData( uint32 rankid )
    {
        auto kfsetting = _kf_rank_config->FindRankSetting( rankid );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 获得排行榜列表
        VectorString zonelist;
        _rank_driver->VectorExecute( zonelist, "smembers %s:%u", KFField::_rank_sort_list.c_str(), rankid );

        for ( auto& strzoneid : zonelist )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );

            auto kfrankdata = _kf_rank_data.Create( RankKey( rankid, zoneid ) );
            kfrankdata->_rank_id = rankid;
            kfrankdata->_zone_id = zoneid;
            kfrankdata->_min_rank_score = 0;
            kfrankdata->_rank_datas.Clear();

            auto& ranksortkey = FormatRankSortKey( rankid, zoneid );
            auto& rankdatakey = FormatRankDataKey( rankid, zoneid );

            VectorString idlist;
            _rank_driver->VectorExecute( idlist, "zrevrange %s 0 %u", ranksortkey.c_str(), kfsetting->_max_count - 1 );
            if ( idlist.empty() )
            {
                continue;
            }

            auto rankindex = 0u;
            for ( auto& strplayerid : idlist )
            {
                auto playerid = KFUtility::ToValue< uint32 >( strplayerid );

                // 读取排行榜信息
                std::string strrankdata = "";
                _rank_driver->StringExecute( strrankdata, "hget %s %u", rankdatakey.c_str(), playerid );

                auto pbrankdata = kfrankdata->_rank_datas.add_rankdata();
                pbrankdata->set_rankindex( ++rankindex );
                KFProto::Parse( pbrankdata, strrankdata, KFCompressEnum::Convert );

                // 加载玩家显示数据
                // todo : 更新的时候pbdata里面包含玩家数据
                // LoadPlayerShowData( pbdatas, playerid );
            }

            // 保存排行榜信息
            SaveRankData( kfrankdata );

            if ( kfsetting->_is_reset_data )
            {
                // 清空数据
                _rank_driver->AppendCommand( "del %s", rankdatakey.c_str() );
                _rank_driver->AppendCommand( "del %s", ranksortkey.c_str() );
                _rank_driver->PipelineExecute();
            }
            else
            {
                // 最小积分
                //auto playerid = idlist.back();
                //std::string minscore = "";
                //_rank_driver->StringExecute( minscore, "zscore %s:%u:%u %u",
                //                             KFField::_rank_sort.c_str(), rankid, zoneid, playerid );
                //kfrankdata->_min_rank_score = KFUtility::ToValue< uint64 >( minscore );

                // 删除指定数量以后的排序
                uint64 rankcount = 0;
                _rank_driver->UInt64Execute( rankcount, "zcard %s", ranksortkey.c_str() );
                if ( rankcount > kfsetting->_max_count )
                {
                    _rank_driver->VoidExecute( "zremrangebyrank %s 0 %u", ranksortkey.c_str(), rankcount - kfsetting->_max_count );
                }
            }
        }
    }

    void KFRankShardModule::LoadPlayerShowData( KFMsg::PBStrings* pbdatas, uint32 playerid )
    {
        //// 查询玩家的信息
        //MapString playervalues;
        //_public_driver->MapExecute( playervalues, "hgetall %s:%u",
        //                            KFField::_public.c_str(), playerid );

        //for ( auto& showdata : _kf_rank_config->_player_data )
        //{
        //    auto pbdata = pbdatas->add_pbstring();
        //    pbdata->set_name( showdata );
        //    pbdata->set_value( playervalues[ showdata ] );
        //}
    }

    // 处理打榜请求
    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleUpdateRankDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateRankDataReq );

        auto& ranksortkey = FormatRankSortKey( kfmsg.rankid(), kfmsg.zoneid() );
        auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), kfmsg.zoneid() );

        auto pbrankdata = &kfmsg.pbrankdata();

        // 排行显示属性
        auto strrankdata = KFProto::Serialize( pbrankdata, KFCompressEnum::Convert );
        _rank_driver->AppendCommand( "hset %s %u %s", rankdatakey.c_str(), kfmsg.playerid(), strrankdata.c_str() );

        // 判断最低的分数
        // if ( IsNeedUpdateRankData( kfmsg.rankid(), kfmsg.zoneid(), kfmsg.rankscore() ) )
        {
            // 添加到排行榜的zone列表
            _rank_driver->AppendCommand( "sadd %s:%u %u", KFField::_rank_sort_list.c_str(),
                                         kfmsg.rankid(), kfmsg.zoneid() );

            // 积分排行列表
            auto strrankscore = __KF_STRING__( pbrankdata->rankscore() );
            _rank_driver->AppendCommand( "zadd %s %s %u", ranksortkey.c_str(), strrankscore.c_str(), kfmsg.playerid() );
        }

        _rank_driver->PipelineExecute();
    }

    bool KFRankShardModule::IsNeedUpdateRankData( uint32 rankid, uint32 zoneid, uint64 rankscore )
    {
        auto kfrankdata = _kf_rank_data.Find( RankKey( rankid, zoneid ) );
        if ( kfrankdata == nullptr )
        {
            return true;
        }

        return rankscore > kfrankdata->_min_rank_score;
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryRanklistReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryRankListReq );

        KFMsg::MsgQueryRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
        auto pbrankdatas = ack.mutable_rankdatas();

        auto kfrankdata = _kf_rank_data.Find( RankKey( kfmsg.rankid(), kfmsg.zoneid() ) );
        if ( kfrankdata != nullptr )
        {
            pbrankdatas->CopyFrom( kfrankdata->_rank_datas );
        }

        _kf_cluster_shard->SendMessageToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_RANK_LIST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFRankShardModule::HandleQueryFriendRanklistReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryFriendRankListReq );

        // 查询
        ListString querysql;
        for ( auto i = 0; i < kfmsg.friendid_size(); ++i )
        {
            auto friendid = kfmsg.friendid( i );
            auto zoneid = KFUtility::CalcZoneId( friendid );

            auto& rankdatakey = FormatRankDataKey( kfmsg.rankid(), zoneid );
            auto strsql = KFUtility::Format( "hget %s %u", rankdatakey.c_str(), friendid );

            querysql.push_back( strsql );
        }

        VectorString rankdatalist;
        _rank_driver->PipelineExecute( querysql, rankdatalist );

        KFMsg::MsgQueryFriendRankListAck ack;
        ack.set_rankid( kfmsg.rankid() );
        auto pbrankdatas = ack.mutable_rankdatas();
        for ( auto& strrankdata : rankdatalist )
        {
            if ( !strrankdata.empty() )
            {
                KFProto::Parse( pbrankdatas->add_rankdata(), strrankdata, KFCompressEnum::Convert );
            }
        }

        _kf_cluster_shard->SendMessageToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_FRIEND_RANK_LIST_ACK, &ack );
    }
}