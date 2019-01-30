#include "KFAttributeModule.hpp"

namespace KFrame
{
    void KFAttributeModule::BeforeRun()
    {
        // 注册command函数
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( adddata ), &KFAttributeModule::OnCommandAddData );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( setdata ), &KFAttributeModule::OnCommandSetData );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( decdata ), &KFAttributeModule::OnCommandDecData );

        _kf_data_client->BindQueryPlayerFunction( this, &KFAttributeModule::OnAfterQueryPlayerData );
        _kf_data_client->BindSetNameFunction( this, &KFAttributeModule::OnAfterSetPlayerName );

        // 注册更新函数
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterSyncAddFunction( this, &KFAttributeModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFAttributeModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFAttributeModule::SendUpdateDataToClient );
        _kf_component->RegisterShowElementFunction( this, &KFAttributeModule::SendElementToClient );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFAttributeModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ, &KFAttributeModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ, &KFAttributeModule::HandleSetNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ, &KFAttributeModule::HandleSetSexReq );
    }

    void KFAttributeModule::BeforeShut()
    {
        // 取消注册command函数
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( adddata ) );
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( setdata ) );
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( decdata ) );

        _kf_data_client->UnBindLoadPlayerFunction( this );
        _kf_data_client->UnBindSetNameFunction( this );

        // 卸载更新函数
        _kf_component->UnRegisterSyncAddFunction();
        _kf_component->UnRegisterSyncRemoveFunction();
        _kf_component->UnRegisterSyncUpdateFunction();
        _kf_component->UnRegisterShowElementFunction();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_SEX_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_COMMAND_FUNCTION__( KFAttributeModule::OnCommandAddData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& strdata = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( strdata, __FUNC_LINE__ );
        if ( ok )
        {
            player->AddElement( __FUNC_LINE__, &kfelements, true );
        }
    }

    __KF_COMMAND_FUNCTION__( KFAttributeModule::OnCommandSetData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& strdata = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( strdata, __FUNC_LINE__ );
        if ( ok )
        {
            kfelements.SetOperate( KFOperateEnum::Set );
            player->AddElement( __FUNC_LINE__, &kfelements, true );
        }
    }

    __KF_COMMAND_FUNCTION__( KFAttributeModule::OnCommandDecData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& stragent = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            player->RemoveElement( __FUNC_LINE__, &kfelements );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAttributeModule::SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncUpdateData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_player->SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFAttributeModule::SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncAddData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_player->SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFAttributeModule::SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncRemoveData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_player->SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFAttributeModule::SendElementToClient( KFEntity* player, const std::string& element )
    {
        KFMsg::MsgShowElement show;
        show.set_element( element );
        _kf_player->SendToClient( player, KFMsg::MSG_SHOW_ELEMENT, &show );
    }
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
        auto ok = _kf_data_client->SetPlayerName( playerid, name, kfmsg.name(), _invalid_int );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::DataServerBusy );
        }
    }

    void KFAttributeModule::OnAfterSetPlayerName( uint32 result, uint64 playerid, const std::string& name, uint64 itemuuid )
    {
        auto player = _kf_player->FindPlayer( playerid );
        if ( player == nullptr )
        {
            return __LOG_ERROR__( "player[{}] set name[{}] item[{}] failed!", playerid, name, itemuuid );
        }

        _kf_display->SendToClient( player, result, name );
        if ( result != KFMsg::NameSetOk )
        {
            return;
        }

        if ( itemuuid != _invalid_int )
        {
            // 删除改名卡
            player->RemoveData( __KF_STRING__( item ), itemuuid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleSetSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFOperateEnum::Set, kfmsg.sex() );
    }

    __KF_MESSAGE_FUNCTION__( KFAttributeModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->UpdateData( kfmsg.dataname(), kfmsg.key(), __KF_STRING__( count ), KFOperateEnum::Dec, kfmsg.count() );

        __LOG_INFO__( "remove data[{}:{}:{}] ok!", kfmsg.dataname(), kfmsg.key(), kfmsg.count() );
    }

}