#include "KFRankClientModule.h"

namespace KFrame
{
    KFRankClientModule::KFRankClientModule()
    {

    }

    KFRankClientModule::~KFRankClientModule()
    {

    }

    void KFRankClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_rank_config, true );
    }

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
        __KF_REMOVE_CONFIG__();
        _kf_component->UnRegisterUpdateDataModule( this );
        ///////////////////////////////////////////////////////////////////////////////////////////

        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_RANK_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_FRIEND_RANK_LIST_REQ );
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFRankClientModule::SendMessageToRank( uint32 rankid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendToStaticObject( __KF_STRING__( rank ), rankid, msgid, message );
    }

    __KF_UPDATE_DATA_FUNCTION__( KFRankClientModule::OnDataUpdateCallBack )
    {
        auto& ranksettinglist = _kf_rank_config->FindRankSetting( kfdata->GetParent()->GetName(), kfdata->GetName() );

        for ( auto kfsetting : ranksettinglist )
        {
            // 属性更新顺序无法保证, 所以先保存到一个列表中, 在AfterRun中更新排行榜数据
            _update_rank_list[ player->GetKeyID() ].push_back( kfsetting );
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

    uint32 KFRankClientModule::CalcRankZoneId( uint32 playerid, const KFRankSetting* kfsetting )
    {
        auto zoneid = _invalid_int;
        if ( kfsetting->_zone_type == KFRankEnum::ZoneRank )
        {
            zoneid = KFUtility::CalcZoneId( playerid );
        }

        return zoneid;
    }

    void KFRankClientModule::UpdateRankDataToShard( KFEntity* player, const KFRankSetting* kfsetting )
    {
        auto kfobject = player->GetData();
        auto playerid = player->GetKeyID();

        // 父属性
        auto kfparent = kfobject;
        if ( !kfsetting->_parent_data.empty() )
        {
            kfparent = kfobject->FindData( kfsetting->_parent_data );
            if ( kfparent == nullptr )
            {
                return;
            }
        }

        // 计算分区id
        auto zoneid = CalcRankZoneId( playerid, kfsetting );

        // 计算排行榜积分
        auto rankscore = CalcRankDataScore( kfparent, kfsetting );

        // 更新到排行榜
        KFMsg::S2SUpdateRankDataReq req;
        req.set_zoneid( zoneid );
        req.set_playerid( playerid );
        req.set_rankid( kfsetting->_rank_id );

        auto pbrankdata = req.mutable_pbrankdata();
        pbrankdata->set_rankindex( 0 );
        pbrankdata->set_playerid( playerid );
        pbrankdata->set_rankscore( rankscore );
        // 显示的数据
        auto pbstrings = pbrankdata->mutable_pbdata();
        for ( auto& showdata : kfsetting->_show_data )
        {
            auto kfshowdata = kfparent->FindData( showdata );
            if ( kfshowdata == nullptr )
            {
                continue;
            }

            auto pbrankdata = pbstrings->add_pbstring();
            pbrankdata->set_name( showdata );
            pbrankdata->set_value( kfshowdata->ToString() );
        }

        // 玩家的基本数据
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        for ( auto& showdata  : _kf_rank_config->_player_data )
        {
            auto kfdata = kfbasic->FindData( showdata );
            if ( kfdata == nullptr )
            {
                continue;
            }

            auto pbrankdata = pbstrings->add_pbstring();
            pbrankdata->set_name( showdata );
            pbrankdata->set_value( kfdata->ToString() );
        }

        SendMessageToRank( kfsetting->_rank_id, KFMsg::S2S_UPDATE_RANK_DATA_REQ, &req );
    }

    uint64 KFRankClientModule::CalcRankDataScore( KFData* kfparent, const KFRankSetting* kfsetting )
    {
#define __RANK_SCORE_MULTIPLE__ 100000

        uint64 rankscore = 0u;
        auto i = 3u;
        for ( auto& showdata : kfsetting->_show_data )
        {
            if ( i == 0 )
            {
                break;
            }

            auto kfdata = kfparent->FindData( showdata );
            if ( kfdata != nullptr )
            {
                auto multiplevalue = pow( __RANK_SCORE_MULTIPLE__, --i );
                rankscore += static_cast< uint64 >( kfdata->GetValue< double >() * multiplevalue );
            }
        }

        return rankscore;
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryRankListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryRankListReq );

        auto kfsetting = _kf_rank_config->FindRankSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryRankListReq req;
        req.set_playerid( playerid );
        req.set_rankid( kfmsg.rankid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );
        req.set_zoneid( CalcRankZoneId( playerid, kfsetting ) );
        auto ok = SendMessageToRank( kfmsg.rankid(), KFMsg::S2S_QUERY_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFRankClientModule::HandleQueryFriendRankListReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryFriendRankListReq );

        auto kfsetting = _kf_rank_config->FindRankSetting( kfmsg.rankid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::RankNotExist );
        }

        KFMsg::S2SQueryFriendRankListReq req;
        req.set_playerid( playerid );
        req.set_rankid( kfmsg.rankid() );
        req.set_serverid( KFGlobal::Instance()->_app_id );

        auto kfobject = player->GetData();
        auto kffriendrecord = kfobject->FindData( __KF_STRING__( friend ) );
        auto kffriend = kffriendrecord->FirstData();
        while ( kffriend != nullptr )
        {
            auto friendid = kffriend->GetValue<uint32>( __KF_STRING__( id ) );
            if ( _invalid_int != friendid )
            {
                req.add_friendid( friendid );
            }

            kffriend = kffriendrecord->NextData();
        }

        auto ok = SendMessageToRank( kfmsg.rankid(), KFMsg::S2S_QUERY_FRIEND_RANK_LIST_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::RankServerBusy );
        }
    }
}
