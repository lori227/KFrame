#include "KFAttributeModule.hpp"

namespace KFrame
{
    void KFAttributeModule::BeforeRun()
    {
        _kf_data_client->BindQueryPlayerFunction( this, &KFAttributeModule::OnAfterQueryPlayerData );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFAttributeModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ, &KFAttributeModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFAttributeModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFAttributeModule::HandleSetSexReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK, &KFAttributeModule::HandleSetPlayerNameToGameAck );
    }

    void KFAttributeModule::BeforeShut()
    {
        _kf_data_client->UnBindLoadPlayerFunction( this );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYERNAME_TO_GAME_ACK );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleQueryPlayerReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryPlayerReq );

        // 不能查询自己的数据，客户端本地可以获取到
        if ( playerid == kfmsg.playerid() )
        {
            return;
        }

        //查询玩家数据
        auto ok = _kf_data_client->QueryPlayerData( playerid, kfmsg.playerid() );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::DataServerBusy );
        }
    }

    void KFAttributeModule::OnAfterQueryPlayerData( uint32 result, uint64 playerid, KFMsg::PBObject* pbplayerdata )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( playerid, result );
        }

        KFMsg::MsgQueryPlayerAck ack;
        ack.mutable_player()->CopyFrom( *pbplayerdata );
        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_PLAYER_ACK, &ack );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFAttributeModule::CheckNameValid( const std::string& name )
    {
        static auto* _option = _kf_option->FindOption( __KF_STRING__( playernamelength ) );
        if ( name.size() > _option->_uint32_value )
        {
            return KFMsg::NameLengthError;
        }

        auto ok = _kf_filter->CheckFilter( name );
        if ( ok )
        {
            return KFMsg::NameFilterError;
        }

        return KFMsg::Ok;
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetNameReq );

        if ( kfmsg.name().empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        auto kfobject = player->GetData();
        auto name = kfobject->GetValue<std::string>( __KF_STRING__( basic ), __KF_STRING__( name ) );
        if ( !name.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::NameAlreadySet );
        }

        // 检查名字的有效性
        auto result = CheckNameValid( kfmsg.name() );
        if ( result != KFMsg::Ok )
        {
            return _kf_display->SendToClient( player, result );
        }

        // 修改名字
        KFMsg::S2SSetPlayerNameToDataReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        req.set_itemuuid( _invalid_int );
        auto ok = _kf_route->SendToRand( playerid, __KF_STRING__( public ), KFMsg::S2S_SET_PLAYERNAME_TO_DATA_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::PublicServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetPlayerNameToGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameToGameAck );

        auto player = _kf_player->FindPlayer( kfmsg.playerid() );
        if ( player == nullptr )
        {
            return __LOG_ERROR__( "player[{}] set name[{}] item[{}] failed!", kfmsg.playerid(), kfmsg.name(), kfmsg.itemuuid() );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.name() );
        if ( kfmsg.result() != KFMsg::NameSetOk )
        {
            return;
        }

        if ( kfmsg.itemuuid() != _invalid_int )
        {
            // 删除改名卡
            player->RemoveData( __KF_STRING__( item ), kfmsg.itemuuid() );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFEnum::Set, kfmsg.sex() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->UpdateData( kfmsg.dataname(), kfmsg.key(), __KF_STRING__( count ), KFEnum::Dec, kfmsg.count() );

        __LOG_INFO__( "remove data[{}:{}:{}] ok!", kfmsg.dataname(), kfmsg.key(), kfmsg.count() );
    }

}