#include "KFRankClientModule.hpp"

namespace KFrame
{
    void KFRankClientModule::BeforeRun()
    {
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
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
        auto parent_data = data->GetParent();
        if ( !data->HaveMask( KFDataDefine::DataMaskRank ) ||
                !parent_data->HaveMask( KFDataDefine::DataMaskRank ) )
        {
            return;
        }

        auto& rank_setting_list = KFRankConfig::Instance()->FindRankSetting( parent_data->_data_setting->_name, key, data->_data_setting->_name );
        for ( auto setting : rank_setting_list )
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

    uint32 KFRankClientModule::CalcRankZoneId( uint64 player_id, std::shared_ptr<const KFRankSetting> setting )
    {
        auto zone_id = _invalid_int;
        if ( setting->_zone_type == KFMsg::ZoneRank )
        {
            zone_id = KFGlobal::Instance()->STUuidZoneId( __STRING__( player ), player_id );
        }

        return zone_id;
    }

    void KFRankClientModule::UpdateRankDataToShard( EntityPtr player, std::shared_ptr<const KFRankSetting> setting )
    {
        // 计算分区id
        auto player_id = player->GetKeyID();
        auto zone_id = CalcRankZoneId( player_id, setting );

        // 计算排行榜积分
        auto rank_score = CalcRankDataScore( player, setting );

        // 更新到排行榜
        KFMsg::S2SUpdateRankDataReq req;
        req.set_zoneid( zone_id );
        req.set_playerid( player_id );
        req.set_rankid( setting->_id );

        auto rank_data = req.mutable_pbrankdata();
        rank_data->set_playerid( player_id );
        rank_data->set_rankscore( rank_score );

        // 显示的数据
        auto data_list = rank_data->mutable_pbdata();
        for ( auto& calc_data : setting->_calc_data )
        {
            auto data = player->Find( calc_data._parent_name, calc_data._child_name );
            if ( data == nullptr )
            {
                data = player->Find( calc_data._child_name );
            }

            if ( data != nullptr )
            {
                ( *data_list )[calc_data._child_name] = data->ToString();
            }
        }

        _kf_route->RepeatToRand( player_id, __STRING__( rank ), KFMsg::S2S_UPDATE_RANK_DATA_REQ, &req );
    }

    uint64 KFRankClientModule::CalcRankDataScore( EntityPtr player, std::shared_ptr<const KFRankSetting> setting )
    {
        // 排行榜积分
        uint64 rank_score = 0u;

        // 倍率系数
        uint64 coefficient = 1u;
        for ( auto iter = setting->_calc_data.rbegin(); iter != setting->_calc_data.rend(); ++iter )
        {
            auto& calc_data = *iter;

            auto data = player->Find( calc_data._parent_name, calc_data._child_name );
            if ( data != nullptr )
            {
                rank_score += data->Get() * coefficient;
            }

            if ( calc_data._max_value > 0u )
            {
                coefficient = coefficient * calc_data._max_value;
            }
        }

        return rank_score;
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryRankListReq, KFMsg::MsgQueryRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryRankListReq req;
        req.set_rankid( kfmsg->rankid() );
        req.set_start( kfmsg->start() == 0u ? 1u : kfmsg->start() );
        req.set_count( kfmsg->count() );
        req.set_zoneid( CalcRankZoneId( player->GetKeyID(), setting ) );
        auto ok = _kf_route->SendToRand( player->GetKeyID(), __STRING__( rank ), KFMsg::S2S_QUERY_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryFriendRankListReq, KFMsg::MsgQueryFriendRankListReq )
    {
        auto setting = KFRankConfig::Instance()->FindSetting( kfmsg->rankid() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryFriendRankListReq req;
        req.set_rankid( kfmsg->rankid() );

        auto friend_record = player->Find( __STRING__( friend ) );
        auto friend_data = friend_record->First();
        while ( friend_data != nullptr )
        {
            req.add_friendid( friend_data->GetKeyID() );
            friend_data = friend_record->Next();
        }

        auto ok = _kf_route->SendToRand( player->GetKeyID(), __STRING__( rank ), KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }
}
