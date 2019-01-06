#include "KFPlayerModule.h"

namespace KFrame
{
    void KFPlayerModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFPlayerModule::OnClientConnectionWorld );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFPlayerModule::TransmitMessageToPlayer );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( shutdown ), &KFPlayerModule::OnDeployShutDownServer );
        _kf_route->RegisterTransmitFunction( this, &KFPlayerModule::TransmitMessageToPlayer );

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
        _kf_component->RegisterShowRewardFunction( this, &KFPlayerModule::SendRewardAgentToClient );

        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ, &KFPlayerModule::HandleKickGamePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &KFPlayerModule::HandlePlayerDisconnectionReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFPlayerModule::HandleLoginOutReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK, &KFPlayerModule::HandleLoadPlayerAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ, &KFPlayerModule::HandleLoginTellTokenToGameReq );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFPlayerModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( shutdown ) );

        _kf_route->UnRegisterTransmitFunction();

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
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOAD_PLAYER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        // 离开游戏世界
        SendLeaveMessageToWord( player );

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

    void KFPlayerModule::SavePlayer( KFEntity* player )
    {
        player->SetNeetToSave( false );

        auto kfobject = player->GetData();
        auto zoneid = KFUtility::CalcZoneId( kfobject->GetKeyID() );
        if ( !player->IsInited() )
        {
            return __LOG_ERROR__( "player[{}:{}] uninit!", zoneid, player->GetKeyID() );
        }

        KFMsg::S2SSavePlayerReq req;
        req.set_zoneid( zoneid );
        req.set_id( kfobject->GetKeyID() );
        _kf_kernel->SerializeToData( kfobject, req.mutable_data() );
        auto ok = _kf_data->SendMessageToData( zoneid, KFMsg::S2S_SAVE_PLAYER_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] save send failed!", zoneid, player->GetKeyID() );
        }
    }

    void KFPlayerModule::SendEnterMessageToWorld( KFEntity* player )
    {
        auto kfobject = player->GetData();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerEnterWorldReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_accountid( kfobject->GetValue( __KF_STRING__( accountid ) ) );
            _kf_game->SendToWorld( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ, &req );
        }
    }

    void KFPlayerModule::SendLeaveMessageToWord( KFEntity* player )
    {
        auto kfobject = player->GetData();

        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerLeaveWorldReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_accountid( kfobject->GetValue( __KF_STRING__( accountid ) ) );
            _kf_game->SendToWorld( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ, &req );
        }
    }

    uint32 KFPlayerModule::GetPlayerCount()
    {
        return _kf_component->GetEntityCount();
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleLoginTellTokenToGameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginTellTokenToGameReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login game req!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉在线玩家
        KickPlayer( pblogin->playerid(), KFMsg::KickEnum::LoginBeKick, __FUNC_LINE__ );

        auto zoneid = KFUtility::CalcZoneId( pblogin->playerid() );

        // 加载玩家数据
        KFMsg::S2SLoginLoadPlayerReq req;
        req.set_zoneid( zoneid );
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = _kf_data->SendMessageToData( zoneid, KFMsg::S2S_LOGIN_LOAD_PLAYER_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}:{}] login game ok!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}:{}] login game failed!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleLoadPlayerAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoadPlayerAck );

        auto result = kfmsg.result();
        auto pblogin = &kfmsg.pblogin();
        auto pbplayerdata = kfmsg.mutable_playerdata();

        __LOG_DEBUG__( "player[{}:{}:{}] load data ack!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        if ( result == KFMsg::Ok )
        {
            // 创建玩家
            auto player = CreatePlayer( pblogin, pbplayerdata );

            // 同步给客户端
            _kf_kernel->SerializeToOnline( player->GetData(), pbplayerdata );
        }

        KFMsg::S2SLoginGameAck ack;
        ack.set_result( result );
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        ack.mutable_playerdata()->CopyFrom( *pbplayerdata );
        ack.set_servertime( KFGlobal::Instance()->_real_time );
        auto ok = _kf_game->SendToGate( pblogin->gateid(), KFMsg::S2S_LOGIN_GAME_ACK, &ack );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}] load game ok!", pblogin->accountid(), pblogin->playerid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}] load game[{}] failed!", pblogin->accountid(), pblogin->playerid(), result );
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
        for ( auto i = 0; i < pbchanneldata->pbstring_size(); ++i )
        {
            auto pbdata = &pbchanneldata->pbstring( i );
            auto kfdata = kfobject->FindData( __KF_STRING__( basic ), pbdata->name() );
            if ( kfdata == nullptr )
            {
                kfdata = kfobject->FindData( pbdata->name() );
                if ( kfdata == nullptr )
                {
                    continue;
                }
            }

            kfdata->SetValue< std::string >( pbdata->value() );
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

        // 进入游戏世界
        SendEnterMessageToWorld( player );
        return player;
    }

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint64 playerid )
    {
        auto kfobject = player->GetData();
        auto kfglobal = KFGlobal::Instance();

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        kfbasic->SetValue( __KF_STRING__( status ), KFMsg::OnlineStatus );
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

    KFEntity* KFPlayerModule::FindPlayer( uint64 playerid, const char* function, uint32 line )
    {
        return _kf_component->FindEntity( playerid, function, line );
    }

    KFEntity* KFPlayerModule::FindPlayer( uint64 playerid )
    {
        return _kf_component->FindEntity( playerid );
    }

    bool KFPlayerModule::SendToClient( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto player = FindPlayer( playerid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return false;
        }

        return SendToClient( player, msgid, message );
    }

    bool KFPlayerModule::SendToClient( uint64 playerid, uint32 msgid, const char* data, uint32 length )
    {
        auto player = FindPlayer( playerid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return false;
        }

        return SendToClient( player, msgid, data, length );
    }

    bool KFPlayerModule::SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto gateid = kfobject->GetValue( __KF_STRING__( gateid ) );
        return _kf_game->SendToClient( gateid, player->GetKeyID(), msgid, message );
    }

    bool KFPlayerModule::SendToClient( KFEntity* player, uint32 msgid, const char* data, uint32 length )
    {
        auto kfobject = player->GetData();
        auto gateid = kfobject->GetValue( __KF_STRING__( gateid ) );
        return _kf_game->SendToClient( gateid, player->GetKeyID(), msgid, data, length );
    }

    bool KFPlayerModule::SendToClient( KFData* kfbasic, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = kfbasic->GetValue( __KF_STRING__( serverid ) );
        auto playerid = kfbasic->GetValue( __KF_STRING__( id ) );
        return _kf_route->SendToRoute( serverid, playerid, msgid, message );
    }

    void KFPlayerModule::SendToGroup( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message, bool sendself )
    {
        // 先发送给自己
        if ( sendself )
        {
            SendToClient( player, msgid, message );
        }

        auto kfobject = player->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );
        auto kfmember = kfmemberrecord->FirstData();
        while ( kfmember != nullptr )
        {
            if ( kfmember->GetKeyID() != player->GetKeyID() )
            {
                auto kfbasic = kfmember->FindData( __KF_STRING__( basic ) );
                SendToClient( kfbasic, msgid, message );
            }

            kfmember = kfmemberrecord->NextData();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFPlayerModule::OnClientConnectionWorld )
    {
        if ( servertype != __KF_STRING__( world ) )
        {
            return;
        }

        // 同步当前在线玩家到World服务器
        KFMsg::S2SGameSyncOnlineReq req;
        auto kfentity = _kf_component->FirstEntity();
        while ( kfentity != nullptr )
        {
            req.add_playerid( kfentity->GetKeyID() );

            kfentity = _kf_component->NextEntity();
        }

        _kf_game->SendToWorld( KFMsg::S2S_GAME_SYNC_ONLINE_REQ, &req );
    }

    __KF_TRANSMIT_FUNCTION__( KFPlayerModule::TransmitMessageToPlayer )
    {
        auto playerid = __KF_DATA_ID__( kfid );
        auto player = FindPlayer( playerid );
        if ( player == nullptr )
        {
            return true;
        }

        return SendToClient( player, msgid, data, length );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandlePlayerDisconnectionReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerDisconnectionReq );

        __LOG_DEBUG__( "player[{}] disconnection!", kfmsg.playerid() );

        _kf_component->RemoveEntity( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleLoginOutReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SLoginOutReq );

        __LOG_DEBUG__( "player[{}] logout!", kfmsg.playerid() );

        for ( auto iter : _player_logout_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

        // 删除玩家
        _kf_component->RemoveEntity( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleKickGamePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickGamePlayerReq );

        auto playerid = kfmsg.playerid();
        KickPlayer( playerid, kfmsg.type(), __FUNC_LINE__ );
    }

    __KF_COMMAND_FUNCTION__( KFPlayerModule::OnDeployShutDownServer )
    {
        auto player = _kf_component->FirstEntity();
        while ( player != nullptr )
        {
            SavePlayer( player );
            player = _kf_component->NextEntity();
        }
    }

    void KFPlayerModule::KickPlayer( uint64 playerid, uint32 type, const char* function, uint32 line )
    {
        auto player = FindPlayer( playerid );
        if ( player == nullptr )
        {
            return;
        }

        __LOG_DEBUG_FUNCTION__( function, line, "kick player[{}] by type[{}]!", playerid, type );

        // 发送消息到proxy
        KFMsg::S2SKickGatePlayerReq req;
        req.set_playerid( playerid );
        req.set_type( type );
        SendToClient( player, KFMsg::S2S_KICK_GATE_PLAYER_REQ, &req );

        // 删除玩家
        _kf_component->RemoveEntity( playerid );
    }

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
        SendToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
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
        SendToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
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
        SendToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendRewardAgentToClient( KFEntity* player, const std::string& reward )
    {
        KFMsg::MsgShowRewardAgent show;
        show.set_reward( reward );
        SendToClient( player, KFMsg::MSG_SHOW_REWARD_AGENT, &show );
    }

    // 判断操作频率
    bool KFPlayerModule::CheckOperateFrequently( KFEntity* player, uint32 time )
    {
        auto kfobject = player->GetData();
        auto operatetime = kfobject->GetValue( __KF_STRING__( operatetime ) );
        if ( KFGlobal::Instance()->_game_time < operatetime )
        {
            _kf_display->SendToClient( player, KFMsg::OperateFrequently );
            return true;
        }

        kfobject->SetValue( __KF_STRING__( operatetime ), KFGlobal::Instance()->_game_time + time );
        return false;
    }
}