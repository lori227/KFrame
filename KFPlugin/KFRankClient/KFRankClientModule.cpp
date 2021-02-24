#include "KFRankClientModule.hpp"

namespace KFrame
{
    void KFRankClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_UPDATE_DATA__( &KFRankClientModule::OnDataUpdateCallBack );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_RANK_LIST_REQ, &KFRankClientModule::HandleQueryRankListReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ, &KFRankClientModule::HandleQueryFriendRankListReq );
    }

    void KFRankClientModule::BeforeShut()
    {
        __UN_UPDATE_DATA__();
        ///////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_QUERY_RANK_LIST_REQ );
        __UN_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFRankClientModule::OnDataUpdateCallBack )
    {
        auto kfparent = kfdata->GetParent();
        if ( !kfdata->HaveMask( KFDataDefine::DataMaskRank ) ||
                !kfparent->HaveMask( KFDataDefine::DataMaskRank ) )
        {
            return;
        }

        auto& ranksettinglist = KFRankConfig::Instance()->FindRankSetting( kfparent->_data_setting->_name, key, kfdata->_data_setting->_name );
        for ( auto setting : ranksettinglist )
        {
            // 属性更新顺序无法保证, 所以先保存到一个列表中, 在AfterRun中更新排行榜数据
            _update_rank_list[ player->GetKeyID() ].insert( setting );
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

            for ( auto setting : iter.second )
            {
                UpdateRankDataToShard( player, setting );
            }
        }

        _update_rank_list.clear();
    }

    uint32 KFRankClientModule::CalcRankZoneId( uint64 player_id, const KFRankSetting* setting )
    {
        auto zone_id = _invalid_int;
        if ( setting->_zone_type == KFMsg::ZoneRank )
        {
            zone_id = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), player_id );
        }

        return zone_id;
    }

    void KFRankClientModule::UpdateRankDataToShard( EntityPtr player, const KFRankSetting* setting )
    {
        // 计算分区id
        auto player_id = player->GetKeyID();
        auto zone_id = CalcRankZoneId( player_id, setting );

        // 计算排行榜积分
        auto rankscore = CalcRankDataScore( player, setting );

        // 更新到排行榜
        KFMsg::S2SUpdateRankDataReq req;
        req.set_zoneid( zone_id );
        req.set_playerid( player_id );
        req.set_rankid( setting->_id );

        auto pbrankdata = req.mutable_pbrankdata();
        pbrankdata->set_playerid( player_id );
        pbrankdata->set_rankscore( rankscore );

        // 显示的数据
        auto pbdatas = pbrankdata->mutable_pbdata();
        for ( auto& calcdata : setting->_calc_data )
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

        _kf_route->RepeatToRand( player_id, __STRING__( rank ), KFMsg::S2S_UPDATE_RANK_DATA_REQ, &req );
    }

    uint64 KFRankClientModule::CalcRankDataScore( EntityPtr player, const KFRankSetting* setting )
    {
        // 排行榜积分
        uint64 rankscore = 0u;

        // 倍率系数
        uint64 coefficient = 1u;
        for ( auto iter = setting->_calc_data.rbegin(); iter != setting->_calc_data.rend(); ++iter )
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

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryRankListReq, KFMsg::MsgQueryRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryRankListReq req;
        req.set_rankid( kfmsg->rankid() );
        req.set_start( kfmsg->start() == 0u ? 1u : kfmsg->start() );
        req.set_count( kfmsg->count() );
        req.set_zoneid( CalcRankZoneId( entity->GetKeyID(), setting ) );
        auto ok = _kf_route->SendToRand( entity->GetKeyID(), __STRING__( rank ), KFMsg::S2S_QUERY_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RankServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryFriendRankListReq, KFMsg::MsgQueryFriendRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryFriendRankListReq req;
        req.set_rankid( kfmsg->rankid() );

        auto kffriendrecord = entity->Find( __STRING__( friend ) );
        auto kffriend = kffriendrecord->First();
        while ( kffriend != nullptr )
        {
            req.add_friendid( kffriend->GetKeyID() );

            kffriend = kffriendrecord->Next();
        }

        auto ok = _kf_route->SendToRand( entity->GetKeyID(), __STRING__( rank ), KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( entity, KFMsg::RankServerBusy );
        }
    }
}
