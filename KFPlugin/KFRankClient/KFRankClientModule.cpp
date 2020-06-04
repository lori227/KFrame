#include "KFRankClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRankClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        _kf_component->RegisterUpdateDataModule( this, &KFRankClientModule::OnDataUpdateCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_RANK_LIST_REQ, &KFRankClientModule::HandleQueryRankListReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ, &KFRankClientModule::HandleQueryFriendRankListReq );
    }

    void KFRankClientModule::BeforeShut()
    {
        _kf_component->UnRegisterUpdateDataModule( this );
        ///////////////////////////////////////////////////////////////////////////////////////////

        __UN_MESSAGE__( KFMsg::MSG_QUERY_RANK_LIST_REQ );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFRankClientModule::OnDataUpdateCallBack )
    {
        auto kfparent = kfdata->GetParent();
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Rank ) ||
                !kfparent->HaveMask( KFDataDefine::Mask_Rank ) )
        {
            return;
        }

        auto& ranksettinglist = KFRankConfig::Instance()->FindRankSetting( kfparent->_data_setting->_name, kfdata->_data_setting->_name );
        for ( auto kfsetting : ranksettinglist )
        {
            // 属性更新顺序无法保证, 所以先保存到一个列表中, 在AfterRun中更新排行榜数据
            _update_rank_list[ player->GetKeyID() ].insert( kfsetting );
        }
    }

    void KFRankClientModule::AfterRun()
    {
        if ( _update_rank_list.empty() )
        {
            return;
        }

        for ( auto& iter : _update_rank_list )
        {
            auto player = _kf_player->FindPlayer( iter.first );
            if ( player == nullptr )
            {
                continue;
            }

            for ( auto kfsetting : iter.second )
            {
                UpdateRankDataToShard( player, kfsetting );
            }
        }

        _update_rank_list.clear();
    }

    uint32 KFRankClientModule::CalcRankZoneId( uint64 playerid, const KFRankSetting* kfsetting )
    {
        auto zoneid = _invalid_int;
        if ( kfsetting->_zone_type == KFMsg::ZoneRank )
        {
            zoneid = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), playerid );
        }

        return zoneid;
    }

    void KFRankClientModule::UpdateRankDataToShard( KFEntity* player, const KFRankSetting* kfsetting )
    {
        // 计算分区id
        auto playerid = player->GetKeyID();
        auto zoneid = CalcRankZoneId( playerid, kfsetting );

        // 计算排行榜积分
        auto rankscore = CalcRankDataScore( player, kfsetting );

        // 更新到排行榜
        KFMsg::S2SUpdateRankDataReq req;
        req.set_zoneid( zoneid );
        req.set_playerid( playerid );
        req.set_rankid( kfsetting->_id );

        auto pbrankdata = req.mutable_pbrankdata();
        pbrankdata->set_playerid( playerid );
        pbrankdata->set_rankscore( rankscore );

        // 显示的数据
        auto pbdatas = pbrankdata->mutable_pbdata();
        for ( auto& calcdata : kfsetting->_calc_data )
        {
            auto kfdata = player->Find( calcdata._parent_name, calcdata._child_name );
            if ( kfdata == nullptr )
            {
                kfdata = player->Find( calcdata._child_name );
            }

            if ( kfdata != nullptr )
            {
                ( *pbdatas )[ calcdata._child_name ] = kfdata->ToString();
            }
        }

        _kf_route->RepeatToRand( playerid, __STRING__( rank ), KFMsg::S2S_UPDATE_RANK_DATA_REQ, &req );
    }

    uint64 KFRankClientModule::CalcRankDataScore( KFEntity* player, const KFRankSetting* kfsetting )
    {
        // 排行榜积分
        uint64 rankscore = 0u;

        // 倍率系数
        uint64 coefficient = 1u;
        for ( auto iter = kfsetting->_calc_data.rbegin(); iter != kfsetting->_calc_data.rend(); ++iter )
        {
            auto& calcdata = *iter;

            auto kfdata = player->Find( calcdata._parent_name, calcdata._child_name );
            if ( kfdata != nullptr )
            {
                rankscore += kfdata->Get() * coefficient;
            }

            if ( calcdata._max_value > 0u )
            {
                coefficient = coefficient * calcdata._max_value;
            }
        }

        return rankscore;
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryRankListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRankListReq );

        auto kfsetting = KFRankConfig::Instance()->FindSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryRankListReq req;
        req.set_rankid( kfmsg.rankid() );
        req.set_zoneid( CalcRankZoneId( playerid, kfsetting ) );
        req.set_start( kfmsg.start() == 0u ? 1u : kfmsg.start() );
        req.set_count( kfmsg.count() );
        auto ok = _kf_route->SendToRand( playerid, __STRING__( rank ), KFMsg::S2S_QUERY_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryFriendRankListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryFriendRankListReq );

        auto kfsetting = KFRankConfig::Instance()->FindSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryFriendRankListReq req;
        req.set_rankid( kfmsg.rankid() );

        auto kffriendrecord = player->Find( __STRING__( friend ) );
        auto kffriend = kffriendrecord->First();
        while ( kffriend != nullptr )
        {
            req.add_friendid( kffriend->GetKeyID() );

            kffriend = kffriendrecord->Next();
        }

        auto ok = _kf_route->SendToRand( playerid, __STRING__( rank ), KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }
}
