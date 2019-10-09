#include "KFRankClientModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFRankClientModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
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
    bool KFRankClientModule::SendMessageToRank( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __KF_STRING__( rank ), msgid, message, true );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_UPDATE_DATA_FUNCTION__( KFRankClientModule::OnDataUpdateCallBack )
    {
        auto kfparent = kfdata->GetParent();
        if ( !kfdata->HaveMask( KFDataDefine::Mask_Rank ) || !kfparent->HaveMask( KFDataDefine::Mask_Rank ) )
        {
            return;
        }

        auto& ranksettinglist = KFRankConfig::Instance()->FindSetting( kfparent->_data_setting->_name, kfdata->_data_setting->_name );
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
            zoneid = KFGlobal::Instance()->STUUIDZoneId( __KF_STRING__( player ), playerid );
        }

        return zoneid;
    }

    void KFRankClientModule::UpdateRankDataToShard( KFEntity* player, const KFRankSetting* kfsetting )
    {
        // 计算分区id
        auto kfobject = player->GetData();
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
        pbrankdata->set_rankindex( 0 );
        pbrankdata->set_playerid( playerid );
        pbrankdata->set_rankscore( rankscore );

        // 显示的数据
        auto pbdatas = pbrankdata->mutable_pbdata();
        for ( auto& calcdata : kfsetting->_calc_data )
        {
            if ( !calcdata.IsShowData() )
            {
                continue;
            }

            auto kfdata = kfobject->FindData( calcdata._parent_name, calcdata._data_name );
            if ( kfdata != nullptr )
            {
                ( *pbdatas )[ calcdata._data_name ] = kfdata->ToString();
            }
        }

        // 玩家的基本数据
        for ( auto& calcdata  : KFRankConfig::Instance()->_player_data )
        {
            auto kfdata = kfobject->FindData( calcdata._parent_name, calcdata._data_name );
            if ( kfdata != nullptr )
            {
                ( *pbdatas )[ calcdata._data_name ] = kfdata->ToString();
            }
        }

        SendMessageToRank( player->GetKeyID(), KFMsg::S2S_UPDATE_RANK_DATA_REQ, &req );
    }

    uint64 KFRankClientModule::CalcRankDataScore( KFEntity* player, const KFRankSetting* kfsetting )
    {
        // 排行榜积分
        uint64 rankscore = 0u;

        // 倍率系数
        uint64 coefficient = 1u;

        auto kfobject = player->GetData();
        for ( auto iter = kfsetting->_calc_data.rbegin(); iter != kfsetting->_calc_data.rend(); ++iter )
        {
            auto& calcdata = *iter;
            if ( !calcdata.IsCalcData() )
            {
                continue;
            }

            auto kfdata = kfobject->FindData( calcdata._parent_name, calcdata._data_name );
            if ( kfdata != nullptr )
            {
                rankscore += static_cast< uint64 >( kfdata->GetValue< double >() * coefficient );
            }

            coefficient = coefficient * calcdata._max_value;
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
        auto ok = SendMessageToRank( playerid, KFMsg::S2S_QUERY_RANK_LIST_REQ, &req );
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

        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriendrecord->FirstData();
        while ( kffriend != nullptr )
        {
            req.add_friendid( kffriend->GetKeyID() );

            kffriend = kffriendrecord->NextData();
        }

        auto ok = SendMessageToRank( playerid, KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }
}
