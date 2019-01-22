#include "KFPlayerModule.h"

namespace KFrame
{
    void KFPlayerModule::BeforeRun()
    {
        // 注册逻辑函数
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->SetEntityDataMask( __NEED_TO_SAVE__ | __DELETE_AND_SAVE__ );
        _kf_component->RegisterEntitySaveFunction( this, &KFPlayerModule::SavePlayer );

        _kf_component->RegisterEntityInitializeFunction( this, &KFPlayerModule::InitPlayer );
        _kf_component->RegisterEntityUninitializeFunction( this, &KFPlayerModule::UnInitPlayer );
        _kf_component->RegisterEntityRunFunction( this, &KFPlayerModule::RunPlayer );

        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFPlayerModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFPlayerModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFPlayerModule::SendUpdateDataToClient );
        _kf_component->RegisterShowRewardFunction( this, &KFPlayerModule::SendRewardElementToClient );

        ////////////////////////////////////////////////////////////////////////////////////////////
        _kf_data->BindLoadPlayerFunction( this, &KFPlayerModule::OnLoadPlayerData );
        _kf_data->BindQueryPlayerFunction( this, &KFPlayerModule::OnQueryPlayerData );
        ////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFPlayerModule::BeforeShut()
    {
        // 卸载逻辑函数
        _kf_component->UnRegisterEntityInitializeFunction();
        _kf_component->UnRegisterEntityUninitializeFunction();
        _kf_component->UnRegisterEntityRunFunction();
        _kf_component->UnRegisterEntityAfterRunFunction();
        _kf_component->UnRegisterEntitySaveFunction();

        // 卸载更新函数
        _kf_component->UnRegisterSyncAddFunction();
        _kf_component->UnRegisterSyncRemoveFunction();
        _kf_component->UnRegisterSyncUpdateFunction();
        _kf_component->UnRegisterShowRewardFunction();
        ////////////////////////////////////////////////////////////////////////////
        _kf_data->UnBindLoadPlayerFunction( this );
        _kf_data->UnBindQueryPlayerFunction( this );

        ////////////////////////////////////////////////////////////////////////////
        _kf_kernel->ReleaseObject( _query_player_data );
    }

    void KFPlayerModule::OnceRun()
    {
        // 创建查询玩家公共数据
        _query_player_data = _kf_kernel->CreateObject( __KF_STRING__( player ) );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::SetAfterLoadFunction( KFLoadPlayerFunction& function )
    {
        _after_load_function = function;
    }

    void KFPlayerModule::SetAfterQueryFunction( KFQueryPlayerFunction& function )
    {
        _after_query_function = function;
    }

    void KFPlayerModule::SetAfterSetNameFunction( KFAfterSetNameFunction& function )
    {
        _after_set_name_function = function;
    }

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

    void KFPlayerModule::AddLogoutFunction( const std::string& moudle, KFEntityFunction& function )
    {
        auto kffunction = _player_logout_function.Create( moudle );
        kffunction->_function = function;
    }

    void KFPlayerModule::RemoveLogoutFunction( const std::string& moudle )
    {
        _player_logout_function.Remove( moudle );
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

    bool KFPlayerModule::SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_game->SendToClient( player, msgid, message );
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

    void KFPlayerModule::RunPlayer( KFEntity* kfentity )
    {
        // 逻辑更新
        for ( auto iter : _player_run_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( kfentity );
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
    bool KFPlayerModule::LoadPlayer( const KFMsg::PBLoginData* pblogin )
    {
        return _kf_data->LoadPlayerData( pblogin );
    }

    bool KFPlayerModule::QueryPlayer( uint64 sendid, uint64 playerid )
    {
        return _kf_data->QueryPlayerData( sendid, playerid );
    }

    void KFPlayerModule::SavePlayer( KFEntity* player )
    {
        if ( !player->IsInited() )
        {
            return;
        }

        // 保存数据库
        static KFMsg::PBObject pbplayerdata;
        _kf_kernel->SerializeToData( player->GetData(), &pbplayerdata );
        auto ok = _kf_data->SavePlayerData( player->GetKeyID(), &pbplayerdata );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] save send failed!", player->GetKeyID() );
        }

        player->SetNeetToSave( !ok );
    }

    void KFPlayerModule::OnLoadPlayerData( uint32 result, const KFMsg::PBLoginData* pblogin, KFMsg::PBObject* pbplayerdata )
    {
        if ( result == KFMsg::Ok )
        {
            // 创建玩家
            auto player = CreatePlayer( pblogin, pbplayerdata );

            // 同步给客户端
            _kf_kernel->SerializeToOnline( player->GetData(), pbplayerdata );
        }

        if ( _after_load_function != nullptr )
        {
            _after_load_function( result, pblogin, pbplayerdata );
        }
    }

    void KFPlayerModule::OnQueryPlayerData( uint32 result, uint64 sendid, KFMsg::PBObject* pbplayerdata )
    {
        if ( result == KFMsg::Ok )
        {
            _query_player_data->Reset();
            _kf_kernel->ParseFromProto( _query_player_data, pbplayerdata );
            _kf_kernel->SerializeToView( _query_player_data, pbplayerdata );
        }

        if ( _after_query_function != nullptr )
        {
            _after_query_function( result, sendid, pbplayerdata );
        }
    }

    KFEntity* KFPlayerModule::CreatePlayer( const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata )
    {
        auto player = _kf_component->CreateEntity( pblogin->playerid(), pbplayerdata );

        auto kfobject = player->GetData();
        kfobject->SetValue( __KF_STRING__( gateid ), pblogin->gateid() );
        kfobject->SetValue( __KF_STRING__( channel ), pblogin->channel() );
        kfobject->SetValue( __KF_STRING__( account ), pblogin->account() );
        kfobject->SetValue( __KF_STRING__( accountid ), pblogin->accountid() );

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

        return player;
    }

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint64 playerid )
    {
        auto kfobject = player->GetData();
        auto kfglobal = KFGlobal::Instance();

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        kfbasic->SetValue( __KF_STRING__( status ), KFMsg::OnlineState );
        kfbasic->SetValue( __KF_STRING__( statustime ), kfglobal->_real_time );
        kfbasic->SetValue( __KF_STRING__( serverid ), kfglobal->_app_id._union._id );

        // 设置名字
        auto name = kfbasic->GetValue< std::string >( __KF_STRING__( name ) );
        auto asciiname = KFConvert::ToAscii( name );
        player->SetName( asciiname );

        // 判断新玩家
        auto basicid = kfbasic->GetValue( __KF_STRING__( id ) );
        if ( basicid == playerid )
        {
            return;
        }

        kfbasic->SetValue( __KF_STRING__( id ), playerid );
        kfobject->SetValue( __KF_STRING__( birthday ), kfglobal->_real_time );

        for ( auto iter : _new_player_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _update_print = 0;
        if ( _update_print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif

        KFMsg::MsgSyncUpdateData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_game->SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFPlayerModule::SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif

        KFMsg::MsgSyncAddData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_game->SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFPlayerModule::SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif

        KFMsg::MsgSyncRemoveData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        _kf_game->SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendRewardElementToClient( KFEntity* player, const std::string& reward )
    {
        KFMsg::MsgShowRewardElement show;
        show.set_reward( reward );
        _kf_game->SendToClient( player, KFMsg::MSG_SHOW_REWARD_ELEMENT, &show );
    }

    // 判断操作频率
    bool KFPlayerModule::CheckOperateFrequently( KFEntity* player, uint32 time )
    {
        auto kfobject = player->GetData();
        auto operatetime = kfobject->GetValue( __KF_STRING__( operatetime ) );
        if ( KFGlobal::Instance()->_game_time < operatetime )
        {
            return true;
        }

        kfobject->SetValue( __KF_STRING__( operatetime ), KFGlobal::Instance()->_game_time + time );
        return false;
    }

    bool KFPlayerModule::SetName( uint64 playerid, const std::string& oldname, const std::string& newname, uint64 itemguid )
    {
        return _kf_data->SetPlayerName( playerid, oldname, newname, itemguid );
    }

    void KFPlayerModule::OnSetName( uint32 result, uint64 playerid, const std::string& name, uint64 itemguid )
    {
        auto player = FindPlayer( playerid );
        if ( player == nullptr )
        {
            if ( result == KFMsg::NameSetOK )
            {
                __LOG_ERROR__( "player={} set name={} itemguid={} failed!", playerid, name, itemguid );
            }

            return;
        }

        auto asciiname = KFConvert::ToAscii( name );
        player->SetName( asciiname );

        // 更新名字
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( name ), name );
        if ( _after_set_name_function != nullptr )
        {
            _after_set_name_function( result, player, name, itemguid );
        }
    }
}