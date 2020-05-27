#include "KFPlayerModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPlayerModule::BeforeRun()
    {
        // 注册逻辑函数
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        _kf_component->RegisterEntityInitializeFunction( this, &KFPlayerModule::InitPlayer );
        _kf_component->RegisterEntityUninitializeFunction( this, &KFPlayerModule::UnInitPlayer );
        _kf_component->RegisterEntityRunFunction( this, &KFPlayerModule::RunPlayer );
        _kf_component->RegisterEntityAfterRunFunction( this, &KFPlayerModule::AfterRunPlayer );

        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFPlayerModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFPlayerModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFPlayerModule::SendUpdateDataToClient );
        _kf_component->RegisterShowElementFunction( this, &KFPlayerModule::SendElementToClient );

        __REGISTER_UPDATE_STRING_2__( __STRING__( basic ), __STRING__( name ), &KFPlayerModule::OnUpdateNameCallBack );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册command函数
        __REGISTER_COMMAND_FUNCTION__( __STRING__( adddata ), &KFPlayerModule::OnCommandAddData );
        __REGISTER_COMMAND_FUNCTION__( __STRING__( setdata ), &KFPlayerModule::OnCommandSetData );
        __REGISTER_COMMAND_FUNCTION__( __STRING__( deldata ), &KFPlayerModule::OnCommandDelData );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFPlayerModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_REQUEST_SYNC_REQ, &KFPlayerModule::HandleRequestSyncReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CANCEL_SYNC_REQ, &KFPlayerModule::HandleCancelSyncReq );

    }

    void KFPlayerModule::BeforeShut()
    {
        // 卸载逻辑函数
        _kf_component->UnRegisterEntityInitializeFunction();
        _kf_component->UnRegisterEntityUninitializeFunction();
        _kf_component->UnRegisterEntityRunFunction();
        _kf_component->UnRegisterEntityAfterRunFunction();

        // 卸载更新函数
        _kf_component->UnRegisterSyncAddFunction();
        _kf_component->UnRegisterSyncRemoveFunction();
        _kf_component->UnRegisterSyncUpdateFunction();
        _kf_component->UnRegisterShowElementFunction();
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
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::AddInitDataFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_init_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveInitDataFunction( const std::string& moudle )
    {
        _player_init_function.Remove( moudle );
    }

    void KFPlayerModule::AddUnInitDataFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_init_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveUnInitDataFunction( const std::string& moudle )
    {
        _player_init_function.Remove( moudle );
    }

    void KFPlayerModule::AddRunDataFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_run_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveRunDataFunction( const std::string& moudle )
    {
        _player_run_function.Remove( moudle );
    }

    void KFPlayerModule::AddAfterRunDataFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_after_run_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveAfterRunDataFunction( const std::string& moudle )
    {
        _player_after_run_function.Remove( moudle );
    }

    void KFPlayerModule::AddResetFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_reset_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveResetFunction( const std::string& moudle )
    {
        _player_reset_function.Remove( moudle );
    }

    void KFPlayerModule::AddBeforeEnterFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_before_enter_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveBeforeEnterFunction( const std::string& moudle )
    {
        _player_before_enter_function.Remove( moudle );
    }

    void KFPlayerModule::AddEnterFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_enter_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveEnterFunction( const std::string& moudle )
    {
        _player_enter_function.Remove( moudle );
    }

    void KFPlayerModule::AddAfterEnterFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_after_enter_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveAfterEnterFunction( const std::string& moudle )
    {
        _player_after_enter_function.Remove( moudle );
    }

    void KFPlayerModule::AddLeaveFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_leave_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveLeaveFunction( const std::string& moudle )
    {
        _player_leave_function.Remove( moudle );
    }

    void KFPlayerModule::AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _new_player_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveNewPlayerFunction( const std::string& moudle )
    {
        _new_player_function.Remove( moudle );
    }

    void KFPlayerModule::AddCreateRoleFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _create_role_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveCreateRoleFunction( const std::string& moudle )
    {
        _create_role_function.Remove( moudle );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////3
    void KFPlayerModule::RemovePlayer()
    {
        std::list< KFEntity* > playerlist;
        for ( auto player = _kf_component->FirstEntity(); player != nullptr; player = _kf_component->NextEntity() )
        {
            playerlist.push_back( player );
        }

        for ( auto player : playerlist )
        {
            _kf_component->RemoveEntity( player );
        }
    }

    void KFPlayerModule::RemovePlayer( uint64 playerid )
    {
        _kf_component->RemoveEntity( playerid );
    }

    void KFPlayerModule::RemovePlayer( KFEntity* player )
    {
        _kf_component->RemoveEntity( player );
    }

    bool KFPlayerModule::SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message, uint32 delay /* = 0 */ )
    {
        if ( !player->IsInited() )
        {
            return false;
        }

        auto gateid = player->Get( __STRING__( gateid ) );
        if ( gateid == _invalid_int )
        {
            return false;
        }

        return _kf_tcp_server->SendNetMessage( gateid, player->GetKeyID(), msgid, message, delay );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::InitPlayer( KFEntity* player )
    {
        // 初始化个模块数据
        for ( auto& iter : _player_init_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    void KFPlayerModule::UnInitPlayer( KFEntity* player )
    {
        auto kfbasic = player->Find( __STRING__( basic ) );
        kfbasic->Set( __STRING__( serverid ), 0 );
        kfbasic->Set( __STRING__( status ), ( uint32 )KFMsg::FrameOfflineStatus );
        kfbasic->Set( __STRING__( statustime ), KFGlobal::Instance()->_real_time );

        // 调用函数, 处理离开游戏的一些事务逻辑
        for ( auto& iter : _player_leave_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        // 卸载
        for ( auto& iter : _player_uninit_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    void KFPlayerModule::RunPlayer( KFEntity* player )
    {
        // 逻辑更新
        for ( auto& iter : _player_run_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    void KFPlayerModule::AfterRunPlayer( KFEntity* player )
    {
        for ( auto& iter : _player_after_run_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFEntity* KFPlayerModule::FindPlayer( uint64 playerid, const char* function, uint32 line )
    {
        return _kf_component->FindEntity( playerid, function, line );
    }

    KFEntity* KFPlayerModule::FindPlayer( uint64 playerid )
    {
        return _kf_component->FindEntity( playerid );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFEntity* KFPlayerModule::CreatePlayer( const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata )
    {
        auto player = _kf_component->CreateEntity( pblogin->playerid(), pbplayerdata );
        if ( player == nullptr )
        {
            return nullptr;
        }

        // 创建玩家
        OnEnterCreatePlayer( player, pblogin->playerid() );

        // 玩家账号信息
        player->Set( __STRING__( ip ), pblogin->ip() );
        player->Set( __STRING__( gateid ), pblogin->gateid() );
        player->Set( __STRING__( channel ), pblogin->channel() );
        player->Set( __STRING__( account ), pblogin->account() );
        player->Set( __STRING__( accountid ), pblogin->accountid() );

        auto kfbasic = player->Find( __STRING__( basic ) );
        kfbasic->Set( __STRING__( serverid ), KFGlobal::Instance()->_app_id->GetId() );
        kfbasic->Set( __STRING__( status ), ( uint32 )KFMsg::FrameOnlineStatus );
        kfbasic->Set( __STRING__( statustime ), KFGlobal::Instance()->_real_time );

        // 渠道数据
        auto pbchanneldata = &pblogin->channeldata();
        for ( auto iter = pbchanneldata->begin(); iter != pbchanneldata->end(); ++iter )
        {
            auto& name = iter->first;
            auto& value = iter->second;

            auto kfdata = player->Find( __STRING__( basic ), name );
            if ( kfdata == nullptr )
            {
                kfdata = player->Find( name );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            kfdata->Set( value );
        }

        // 调用重置函数
        for ( auto& iter : _player_reset_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
        // 调用函数, 处理进入游戏的一些事务逻辑
        for ( auto& iter : _player_before_enter_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
        for ( auto& iter : _player_enter_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
        for ( auto& iter : _player_after_enter_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        player->SetInited();
        player->SetNew( false );
        return player;
    }

    __KF_UPDATE_STRING_FUNCTION__( KFPlayerModule::OnUpdateNameCallBack )
    {
        if ( oldvalue == _invalid_string )
        {
            for ( auto& iter : _create_role_function._objects )
            {
                auto kffunction = iter.second;
                kffunction->_function( player );
            }

            for ( auto elements : KFPlayerConfig::Instance()->_new_role_elements )
            {
                player->AddElement( elements, _default_multiple, __STRING__( newrole ), 0u, __FUNC_LINE__ );
            }
        }
    }

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint64 playerid )
    {
        // 判断新玩家
        auto kfglobal = KFGlobal::Instance();
        auto basicid = player->Get( __STRING__( basic ), __STRING__( id ) );
        if ( basicid == playerid )
        {
            return;
        }

        player->InitData();
        player->SetNew( true );
        player->Set( __STRING__( birthday ), kfglobal->_real_time );
        player->Set( __STRING__( basic ), __STRING__( id ), playerid );

        for ( auto& iter : _new_player_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        // 新玩家添加道具
        for ( auto elements : KFPlayerConfig::Instance()->_new_player_elements )
        {
            player->AddElement( elements, _default_multiple, __STRING__( newplayer ), 0u, __FUNC_LINE__ );
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

        auto& strdata = params[ 0 ];

        KFElements kfelements;
        auto ok = kfelements.Parse( strdata, __FUNC_LINE__ );
        if ( ok )
        {
            kfelements.SetOperate( KFEnum::Add );
            player->AddElement( &kfelements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandSetData )
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
            kfelements.SetOperate( KFEnum::Set );
            player->AddElement( &kfelements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandDelData )
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
            kfelements.SetOperate( KFEnum::Dec );
            player->RemoveElement( &kfelements, _default_multiple, __STRING__( command ), 0u, __FUNC_LINE__ );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    static uint32 _print = 0u;
    void KFPlayerModule::SendUpdateDataToClient( KFEntity* player, KFMsg::PBObject& pbobject )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1u )
        {
            auto temp = pbobject.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncUpdateData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFPlayerModule::SendAddDataToClient( KFEntity* player, KFMsg::PBObject& pbobject )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1 )
        {
            auto temp = pbobject.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncAddData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFPlayerModule::SendRemoveDataToClient( KFEntity* player, KFMsg::PBObject& pbobject )
    {
#ifdef __KF_DEBUG__
        if ( _print == 1 )
        {
            auto temp = pbobject.DebugString();
            __LOG_DEBUG__( "player=[{}], data={}", player->GetKeyID(), temp );
        }
#endif

        KFMsg::MsgSyncRemoveData sync;
        sync.mutable_pbdata()->Swap( &pbobject );
        SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendElementToClient( KFEntity* player, KFMsg::PBShowElements& pbelements )
    {
        KFMsg::MsgShowElement show;
        show.mutable_elements()->Swap( &pbelements );
        SendToClient( player, KFMsg::MSG_SHOW_ELEMENT, &show );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->RemoveData( kfmsg.dataname(), kfmsg.key() );
        __LOG_INFO__( "remove data[{}:{}] ok", kfmsg.dataname(), kfmsg.key() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRequestSyncReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRequestSyncReq );

        auto kfdata = player->Find( kfmsg.dataname() );
        if ( kfdata == nullptr )
        {
            return;
        }

        kfdata->AddMask( KFDataDefine::Mask_Client );
        switch (  kfdata->_data_setting->_type )
        {
        case KFDataDefine::Type_Object:
            player->SyncUpdateData( kfdata, kfdata->GetKeyID() );
            break;
        case KFDataDefine::Type_Record:
            player->SynAddRecordData( kfdata );
            break;
        default:
            break;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleCancelSyncReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCancelSyncReq );

        auto kfdata = player->Find( kfmsg.dataname() );
        if ( kfdata == nullptr )
        {
            return;
        }

        kfdata->RemoveMask( KFDataDefine::Mask_Client );
    }
}