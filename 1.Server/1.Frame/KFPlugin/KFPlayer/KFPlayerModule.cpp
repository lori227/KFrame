#include "KFPlayerModule.hpp"

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
    }

    void KFPlayerModule::BeforeShut()
    {
        // 卸载逻辑函数
        _kf_component->UnRegisterEntityInitializeFunction();
        _kf_component->UnRegisterEntityUninitializeFunction();
        _kf_component->UnRegisterEntityRunFunction();
        _kf_component->UnRegisterEntityAfterRunFunction();
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
        player->UpdateData( __KF_STRING__( totaltime ), KFOperateEnum::Add, kfglobal->_real_time - onlinetime );

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

        return player;
    }

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint64 playerid )
    {
        auto kfobject = player->GetData();
        auto kfglobal = KFGlobal::Instance();

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        kfbasic->SetValue( __KF_STRING__( serverid ), kfglobal->_app_id._union._id );

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
}