#include "KFPlayerModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPlayerModule::BeforeRun()
    {
        // 注册逻辑函数
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->RegisterEntityInitializeFunction( this, &KFPlayerModule::InitPlayer );
        _kf_component->RegisterEntityUninitializeFunction( this, &KFPlayerModule::UnInitPlayer );
        _kf_component->RegisterEntityRunFunction( this, &KFPlayerModule::RunPlayer );
        _kf_component->RegisterEntityAfterRunFunction( this, &KFPlayerModule::AfterRunPlayer );

        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFPlayerModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFPlayerModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFPlayerModule::SendUpdateDataToClient );
        _kf_component->RegisterShowElementFunction( this, &KFPlayerModule::SendElementToClient );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 注册command函数
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( adddata ), &KFPlayerModule::OnCommandAddData );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( setdata ), &KFPlayerModule::OnCommandSetData );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( decdata ), &KFPlayerModule::OnCommandDecData );

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
        // 取消注册command函数
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( adddata ) );
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( setdata ) );
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( decdata ) );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REQUEST_SYNC_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CANCEL_SYNC_REQ );
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

    void KFPlayerModule::AddEnterFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_enter_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveEnterFunction( const std::string& moudle )
    {
        _player_enter_function.Remove( moudle );
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////3
    KFEntity* KFPlayerModule::FirstPlayer()
    {
        return _kf_component->FirstEntity();
    }

    KFEntity* KFPlayerModule::NextPlayer()
    {
        return _kf_component->NextEntity();
    }

    void KFPlayerModule::RemovePlayer( uint64 playerid )
    {
        _kf_component->RemoveEntity( playerid );
    }

    void KFPlayerModule::RemovePlayer( KFEntity* player )
    {
        _kf_component->RemoveEntity( player );
    }

    bool KFPlayerModule::SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto gateid = kfobject->GetValue( __KF_STRING__( gateid ) );
        return _kf_tcp_server->SendNetMessage( gateid, player->GetKeyID(), msgid, message );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::InitPlayer( KFEntity* player )
    {
        // 初始化个模块数据
        for ( auto iter : _player_init_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    void KFPlayerModule::UnInitPlayer( KFEntity* player )
    {
        auto kfglobal = KFGlobal::Instance();

        auto kfobject = player->GetData();
        kfobject->SetValue( __KF_STRING__( offlinetime ), kfglobal->_real_time );

        // 更新总时间
        auto onlinetime = kfobject->GetValue( __KF_STRING__( onlinetime ) );
        player->UpdateData( __KF_STRING__( totaltime ), KFEnum::Add, kfglobal->_real_time - onlinetime );

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
        for ( auto iter : _player_run_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    void KFPlayerModule::AfterRunPlayer( KFEntity* player )
    {
        for ( auto iter : _player_after_run_function._objects )
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

        auto kfobject = player->GetData();
        kfobject->SetValue( __KF_STRING__( gateid ), pblogin->gateid() );
        kfobject->SetValue( __KF_STRING__( channel ), pblogin->channel() );
        kfobject->SetValue( __KF_STRING__( account ), pblogin->account() );
        kfobject->SetValue( __KF_STRING__( accountid ), pblogin->accountid() );
        kfobject->SetValue( __KF_STRING__( onlinetime ), KFGlobal::Instance()->_real_time );

        // 渠道数据
        auto pbchanneldata = &pblogin->channeldata();
        for ( auto iter = pbchanneldata->begin(); iter != pbchanneldata->end(); ++iter )
        {
            auto& name = iter->first;
            auto& value = iter->second;

            auto kfdata = kfobject->FindData( __KF_STRING__( basic ), name );
            if ( kfdata == nullptr )
            {
                kfdata = kfobject->FindData( name );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            kfdata->SetValue( value );
        }

        // 创建玩家
        OnEnterCreatePlayer( player, pblogin->playerid() );

        // 调用重置函数
        for ( auto iter : _player_reset_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        // 调用函数, 处理进入游戏的一些事务逻辑
        for ( auto iter : _player_enter_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        player->SetInited();
        return player;
    }

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint64 playerid )
    {
        auto kfobject = player->GetData();
        auto kfglobal = KFGlobal::Instance();

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        kfbasic->SetValue( __KF_STRING__( serverid ), kfglobal->_app_id->GetId() );

        // 设置名字
        player->SetName( kfbasic->GetValue< std::string >( __KF_STRING__( name ) ) );

        // 判断新玩家
        auto basicid = kfbasic->GetValue( __KF_STRING__( id ) );
        if ( basicid == playerid )
        {
            return;
        }

        kfbasic->SetValue( __KF_STRING__( id ), playerid );
        kfbasic->SetValue( __KF_STRING__( birthday ), kfglobal->_real_time );

        for ( auto iter : _new_player_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
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
            player->AddElement( &kfelements, true, __FUNC_LINE__ );
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
            player->AddElement( &kfelements, true, __FUNC_LINE__ );
        }
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnCommandDecData )
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
            player->RemoveElement( &kfelements, __FUNC_LINE__ );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
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
        SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFPlayerModule::SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
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
        SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFPlayerModule::SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
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
        SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendElementToClient( KFEntity* player, const KFMsg::PBShowElement& pbelement )
    {
        KFMsg::MsgShowElement show;
        show.mutable_element()->CopyFrom( pbelement );
        SendToClient( player, KFMsg::MSG_SHOW_ELEMENT, &show );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->RemoveData( kfmsg.dataname(), kfmsg.key() );
        __LOG_INFO__( "remove data[{}:{}] ok!", kfmsg.dataname(), kfmsg.key() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRequestSyncReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRequestSyncReq );

        auto kfobject = player->GetData();
        auto kfdata = kfobject->FindData( kfmsg.dataname() );
        if ( kfdata == nullptr )
        {
            return;
        }

        kfdata->AddMask( KFDataDefine::Mask_Client );
        if ( kfdata->GetType() == KFDataDefine::Type_Object )
        {
            player->SyncUpdateData( kfdata, kfdata->GetKeyID() );
        }
        else if ( kfdata->GetType() == KFDataDefine::Type_Record )
        {
            for ( auto kfchild = kfdata->FirstData(); kfchild != nullptr; kfchild = kfdata->NextData() )
            {
                player->SyncAddData( kfchild, kfchild->GetKeyID() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleCancelSyncReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCancelSyncReq );

        auto kfobject = player->GetData();
        auto kfdata = kfobject->FindData( kfmsg.dataname() );
        if ( kfdata == nullptr )
        {
            return;
        }

        kfdata->RemoveMask( KFDataDefine::Mask_Client );
    }
}