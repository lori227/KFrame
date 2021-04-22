#include "KFPlayerModule.hpp"

namespace KFrame
{
    namespace NewPlayerEnum
    {
        enum EConstEnum
        {
            NewAccount = 1,	// 新建账号
            NewRole = 2,	// 新建角色
        };
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::BeforeRun()
    {
        __REGISTER_FIND_ENTITY__( KFMessageEnum::Player, &KFPlayerModule::FindPlayer );

        // 注册逻辑函数
        _component = _kf_kernel->FindComponent( __STRING__( player ) );
        _component->RegisterEntityRunFunction( this, &KFPlayerModule::RunPlayer );
        _component->RegisterEntityAfterRunFunction( this, &KFPlayerModule::AfterRunPlayer );

        // 注册更新函数
        _component->RegisterSyncAddFunction( this, &KFPlayerModule::SendAddDataToClient );
        _component->RegisterSyncRemoveFunction( this, &KFPlayerModule::SendRemoveDataToClient );
        _component->RegisterSyncUpdateFunction( this, &KFPlayerModule::SendUpdateDataToClient );
        _component->RegisterShowElementFunction( this, &KFPlayerModule::SendElementToClient );

        __REGISTER_UPDATE_STRING_2__( __STRING__( basic ), __STRING__( name ), &KFPlayerModule::OnUpdateNameCallBack );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册command函数
        __REGISTER_COMMAND_FUNCTION__( __STRING__( adddata ), &KFPlayerModule::OnCommandAddData );
        __REGISTER_COMMAND_FUNCTION__( __STRING__( setdata ), &KFPlayerModule::OnCommandSetData );
        __REGISTER_COMMAND_FUNCTION__( __STRING__( deldata ), &KFPlayerModule::OnCommandDelData );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_REMOVE_DATA_REQ, &KFPlayerModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_REQUEST_SYNC_REQ, &KFPlayerModule::HandleRequestSyncReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_CANCEL_SYNC_REQ, &KFPlayerModule::HandleCancelSyncReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_UPDATE_INT_REQ, &KFPlayerModule::HandleUpdateIntReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_UPDATE_STR_REQ, &KFPlayerModule::HandleUpdateStrReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_SYNC_UPDATE_DATA_FROM_SERVER, &KFPlayerModule::HandleSyncUpdateDataFromServerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_SYNC_ADD_DATA_FROM_SERVER, &KFPlayerModule::HandleSyncAddDataFromServerReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_SYNC_REMOVE_DATA_FROM_SERVER, &KFPlayerModule::HandleSyncRemoveDataFromServerReq );

    }

    void KFPlayerModule::BeforeShut()
    {
        __UN_TIMER_0__();
        __UN_FIND_ENTITY__( KFMessageEnum::Player );

        // 卸载逻辑函数
        _component->UnRegisterEntityRunFunction();
        _component->UnRegisterEntityAfterRunFunction();

        // 卸载更新函数
        _component->UnRegisterSyncAddFunction();
        _component->UnRegisterSyncRemoveFunction();
        _component->UnRegisterSyncUpdateFunction();
        _component->UnRegisterShowElementFunction();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_UPDATE_STRING_2__( __STRING__( basic ), __STRING__( name ) );
        // 取消注册command函数
        __UN_COMMAND_FUNCTION__( __STRING__( adddata ) );
        __UN_COMMAND_FUNCTION__( __STRING__( setdata ) );
        __UN_COMMAND_FUNCTION__( __STRING__( deldata ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UN_MESSAGE__( KFMsg::MSG_REQUEST_SYNC_REQ );
        __UN_MESSAGE__( KFMsg::MSG_CANCEL_SYNC_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SYNC_UPDATE_DATA_FROM_SERVER );
        __UN_MESSAGE__( KFMsg::S2S_SYNC_ADD_DATA_FROM_SERVER );
        __UN_MESSAGE__( KFMsg::S2S_SYNC_REMOVE_DATA_FROM_SERVER );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::AddRunDataFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_run_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveRunDataFunction( KFModule* module )
    {
        _player_run_function.Remove( module );
    }

    void KFPlayerModule::AddAfterRunDataFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_after_run_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveAfterRunDataFunction( KFModule* module )
    {
        _player_after_run_function.Remove( module );
    }

    void KFPlayerModule::AddResetFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_reset_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveResetFunction( KFModule* module )
    {
        _player_reset_function.Remove( module );
    }

    void KFPlayerModule::AddBeforeEnterFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_before_enter_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveBeforeEnterFunction( KFModule* module )
    {
        _player_before_enter_function.Remove( module );
    }

    void KFPlayerModule::AddEnterFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_enter_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveEnterFunction( KFModule* module )
    {
        _player_enter_function.Remove( module );
    }

    void KFPlayerModule::AddAfterEnterFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_after_enter_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveAfterEnterFunction( KFModule* module )
    {
        _player_after_enter_function.Remove( module );
    }

    void KFPlayerModule::AddLeaveFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _player_leave_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveLeaveFunction( KFModule* module )
    {
        _player_leave_function.Remove( module );
    }

    void KFPlayerModule::AddNewPlayerFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _new_player_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveNewPlayerFunction( KFModule* module )
    {
        _new_player_function.Remove( module );
    }

    void KFPlayerModule::AddCreateRoleFunction( KFModule* module, KFEntityFunction& function )
    {
        auto function_data = _create_role_function.Create( module );
        function_data->SetFunction( module, function );
    }

    void KFPlayerModule::RemoveCreateRoleFunction( KFModule* module )
    {
        _create_role_function.Remove( module );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////3
    void KFPlayerModule::Logout()
    {
        std::list<EntityPtr> player_list;
        for ( auto player = _component->FirstEntity(); player != nullptr; player = _component->NextEntity() )
        {
            player_list.push_back( player );
        }

        for ( auto player : player_list )
        {
            Logout( player );
        }
    }

    void KFPlayerModule::Logout( uint64 player_id )
    {
        auto player = FindPlayer( player_id );
        if ( player != nullptr )
        {
            Logout( player );
        }
    }

    void KFPlayerModule::Logout( EntityPtr player )
    {
        auto basic_data = player->Find( __STRING__( basic ) );
        basic_data->Operate( __STRING__( serverid ), KFEnum::Set, 0 );
        basic_data->Operate( __STRING__( status ), KFEnum::Set, ( uint32 )KFMsg::OfflineStatus );
        basic_data->Operate( __STRING__( statustime ), KFEnum::Set, KFGlobal::Instance()->_real_time );

        // 调用函数, 处理离开游戏的一些事务逻辑
        for ( auto& iter : _player_leave_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
    }

    bool KFPlayerModule::SendToClient( EntityPtr player, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay /* = 0 */ )
    {
        if ( !player->IsInited() )
        {
            return false;
        }

        auto gate_id = player->Get( __STRING__( gateid ) );
        if ( gate_id == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( gate_id, player->GetKeyID(), msg_id, message, delay );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::RunPlayer( EntityPtr player )
    {
        // 逻辑更新
        for ( auto& iter : _player_run_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
    }

    void KFPlayerModule::AfterRunPlayer( EntityPtr player )
    {
        for ( auto& iter : _player_after_run_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EntityPtr KFPlayerModule::FindPlayer( uint64 player_id )
    {
        return _component->FindEntity( player_id );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    EntityPtr KFPlayerModule::Login( const KFMsg::PBLoginData* login_data, const KFMsg::PBObject* player_data )
    {
        auto player = _component->CreateEntity( login_data->playerid(), player_data );
        if ( player == nullptr )
        {
            return nullptr;
        }

        // 创建玩家
        OnEnterCreatePlayer( player, login_data->playerid() );

        // 玩家账号信息
        player->Operate( __STRING__( ip ), KFEnum::Set, login_data->ip() );
        player->Operate( __STRING__( gateid ), KFEnum::Set, login_data->gateid() );
        player->Operate( __STRING__( channel ), KFEnum::Set, login_data->channel() );
        player->Operate( __STRING__( account ), KFEnum::Set, login_data->account() );
        player->Operate( __STRING__( accountid ), KFEnum::Set, login_data->accountid() );

        auto basic_data = player->Find( __STRING__( basic ) );
        basic_data->Operate( __STRING__( status ), KFEnum::Set, ( uint32 )KFMsg::OnlineStatus );
        basic_data->Operate( __STRING__( statustime ), KFEnum::Set, KFGlobal::Instance()->_real_time );
        basic_data->Operate( __STRING__( serverid ), KFEnum::Set, KFGlobal::Instance()->_app_id->GetId() );

        // 渠道数据
        auto channel_data = &login_data->channeldata();
        for ( auto iter = channel_data->begin(); iter != channel_data->end(); ++iter )
        {
            auto& name = iter->first;
            auto& value = iter->second;

            auto data = player->Find( __STRING__( basic ), name );
            if ( data == nullptr )
            {
                data = player->Find( name );
                if ( data == nullptr )
                {
                    continue;
                }
            }

            data->Set( value );
        }

        // 调用重置函数
        for ( auto& iter : _player_reset_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
        // 调用函数, 处理进入游戏的一些事务逻辑
        for ( auto& iter : _player_before_enter_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
        for ( auto& iter : _player_enter_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }
        for ( auto& iter : _player_after_enter_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }

        player->SetInited();
        player->SetNewEntity( false );

        // 启动延迟上线数据同步
        StartSyncOnlineTimer( player );
        return player;
    }

    EntityPtr KFPlayerModule::ReLogin( uint64 player_id, uint64 gate_id )
    {
        auto player = FindPlayer( player_id );
        if ( player == nullptr )
        {
            return nullptr;
        }

        player->Operate( __STRING__( gateid ), KFEnum::Set, gate_id );

        // 启动延迟上线数据同步
        StartSyncOnlineTimer( player );
        return player;
    }

    void KFPlayerModule::StartSyncOnlineTimer( EntityPtr player )
    {
        for ( auto delay_time : player->_data_setting->_class_setting->_online_sync_time )
        {
            if ( delay_time > 0u )
            {
                __COUNT_TIMER_2__( player->GetKeyID(), delay_time, delay_time, 1u, &KFPlayerModule::OnTimerSyncEntityToOnline );
            }
        }
    }

    __KF_TIMER_FUNCTION__( KFPlayerModule::OnTimerSyncEntityToOnline )
    {
        auto player = FindPlayer( object_id );
        if ( player == nullptr )
        {
            return;
        }

        KFMsg::MsgSyncOnlineData sync;
        auto player_data = _kf_kernel->SerializeToOnline( player, data_id );
        sync.mutable_pbdata()->CopyFrom( *player_data );
        SendToClient( player, KFMsg::MSG_SYNC_ONLINE_DATA, &sync );
    }

    __KF_UPDATE_STRING_FUNCTION__( KFPlayerModule::OnUpdateNameCallBack )
    {
        if ( old_value != _invalid_string )
        {
            return;
        }

        for ( auto& iter : _create_role_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }

        for ( auto& iter : KFPlayerConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_type == NewPlayerEnum::NewRole )
            {
                player->AddElement( &setting->_reward, _default_multiple, __STRING__( newrole ), 0u, __FUNC_LINE__ );
            }
        }
    }

    void KFPlayerModule::OnEnterCreatePlayer( EntityPtr player, uint64 player_id )
    {
        // 判断新玩家
        auto global = KFGlobal::Instance();
        auto basic_id = player->Get( __STRING__( basic ), __STRING__( id ) );
        if ( basic_id == player_id )
        {
            return;
        }

        player->InitData();
        player->SetNewEntity( true );
        player->Operate( __STRING__( birthday ), KFEnum::Set, global->_real_time );
        player->Operate( __STRING__( basic ), __STRING__( id ), KFEnum::Set, player_id );

        for ( auto& iter : _new_player_function._objects )
        {
            auto function_data = iter.second;
            function_data->Call( player );
        }

        // 新玩家添加道具
        for ( auto& iter : KFPlayerConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( setting->_type == NewPlayerEnum::NewAccount )
            {
                player->AddElement( &setting->_reward, _default_multiple, __STRING__( newplayer ), 0u, __FUNC_LINE__ );
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandAddData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = params[0];
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            elements.SetOperate( KFEnum::Add );
            player->AddElement( &elements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandSetData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = params[0];
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            elements.SetOperate( KFEnum::Set );
            player->AddElement( &elements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandDelData )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = params[0];
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            elements.SetOperate( KFEnum::Dec );
            player->RemoveElement( &elements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static uint32 _print = 0u;
    void KFPlayerModule::SendUpdateDataToClient( EntityPtr player, KFMsg::PBObject& message )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1u )
        {
            auto temp = message.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncUpdateData sync;
        sync.mutable_pbdata()->Swap( &message );
        SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFPlayerModule::SendAddDataToClient( EntityPtr player, KFMsg::PBObject& message )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1 )
        {
            auto temp = message.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncAddData sync;
        sync.mutable_pbdata()->Swap( &message );
        SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFPlayerModule::SendRemoveDataToClient( EntityPtr player, KFMsg::PBObject& message )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1 )
        {
            auto temp = message.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncRemoveData sync;
        sync.mutable_pbdata()->Swap( &message );
        SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendElementToClient( EntityPtr player, KFMsg::PBShowElements& show_elements )
    {
        KFMsg::MsgShowElement show;
        show.mutable_elements()->Swap( &show_elements );
        SendToClient( player, KFMsg::MSG_SHOW_ELEMENT, &show );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRemoveDataReq, KFMsg::MsgRemoveDataReq )
    {
        player->RemoveRecord( kfmsg->data_name(), kfmsg->key() );
        __LOG_INFO__( "remove data[{}:{}] ok", kfmsg->data_name(), kfmsg->key() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRequestSyncReq, KFMsg::MsgRequestSyncReq )
    {
        auto data = player->Find( kfmsg->data_name() );
        if ( data == nullptr )
        {
            return;
        }

        data->AddMask( KFDataDefine::DataMaskSync );
        switch ( data->_data_setting->_type )
        {
        case KFDataDefine::DataTypeObject:
            player->SyncUpdateDataToClient( data, data->GetKeyID() );
            break;
        case KFDataDefine::DataTypeRecord:
            player->SynAddRecordDataToClient( data );
            break;
        default:
            break;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleCancelSyncReq, KFMsg::MsgCancelSyncReq )
    {
        auto data = player->Find( kfmsg->data_name() );
        if ( data == nullptr )
        {
            return;
        }

        data->RemoveMask( KFDataDefine::DataMaskSync );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleUpdateIntReq, KFMsg::MsgUpdateIntReq )
    {
        // 1级属性
        if ( kfmsg->parent_name().empty() )
        {
            auto data = player->Find( kfmsg->data_name() );
            if ( data != nullptr && data->HaveMask( KFDataDefine::DataMaskClient ) )
            {
                player->UpdateData( data, kfmsg->operate(), kfmsg->value() );
            }

            return;
        }

        // 2级object属性
        if ( kfmsg->key() == 0u )
        {
            auto data = player->Find( kfmsg->parent_name(), kfmsg->data_name() );
            if ( data != nullptr && data->HaveMask( KFDataDefine::DataMaskClient ) )
            {
                player->UpdateData( data, kfmsg->operate(), kfmsg->value() );
            }

            return;
        }

        // 3级record属性
        auto parent_data = player->Find( kfmsg->parent_name() );
        if ( parent_data == nullptr || !parent_data->_data_setting->HaveMask( KFDataDefine::DataMaskClient ) )
        {
            return;
        }

        player->UpdateRecord( parent_data, kfmsg->key(), kfmsg->data_name(), kfmsg->operate(), kfmsg->value() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleUpdateStrReq, KFMsg::MsgUpdateStrReq )
    {
        // 1级属性
        if ( kfmsg->parent_name().empty() )
        {
            auto data = player->Find( kfmsg->data_name() );
            if ( data != nullptr && data->HaveMask( KFDataDefine::DataMaskClient ) )
            {
                player->UpdateData( data, kfmsg->value() );
            }

            return;
        }

        // 2级object属性
        if ( kfmsg->key() == 0u )
        {
            auto data = player->Find( kfmsg->parent_name(), kfmsg->data_name() );
            if ( data != nullptr && data->HaveMask( KFDataDefine::DataMaskClient ) )
            {
                player->UpdateData( data, kfmsg->value() );
            }

            return;
        }

        // 3级record属性
        auto parent_data = player->Find( kfmsg->parent_name() );
        if ( parent_data == nullptr )
        {
            return;
        }

        auto data_setting = parent_data->_data_setting->_class_setting->FindSetting( kfmsg->data_name() );
        if ( data_setting == nullptr || !data_setting->HaveMask( KFDataDefine::DataMaskClient ) )
        {
            return;
        }

        player->UpdateRecord( kfmsg->parent_name(), kfmsg->key(), kfmsg->data_name(), kfmsg->value() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleSyncUpdateDataFromServerReq, KFMsg::S2SSyncUpdateDataFromServer )
    {
        auto object_data = player->Find( kfmsg->data_name() );
        if ( object_data == nullptr )
        {
            return;
        }

        player->SyncUpdateDataFromServer( object_data, &kfmsg->pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleSyncAddDataFromServerReq, KFMsg::S2SSyncAddDataFromServer )
    {
        auto object_data = player->Find( kfmsg->data_name() );
        if ( object_data == nullptr )
        {
            return;
        }

        player->SyncAddDataFromServer( object_data, &kfmsg->pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleSyncRemoveDataFromServerReq, KFMsg::S2SSyncRemoveDataFromServer )
    {
        auto object_data = player->Find( kfmsg->data_name() );
        if ( object_data == nullptr )
        {
            return;
        }

        player->SyncRemoveDataFromServer( object_data, &kfmsg->pbdata() );
    }
}