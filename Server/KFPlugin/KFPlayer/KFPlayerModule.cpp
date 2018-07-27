#include "KFPlayerModule.h"
#include "KFConvert/KFConvert.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFPlayerModule::KFPlayerModule()
    {
        _kf_component = nullptr;
        _kf_player_data = nullptr;
    }

    KFPlayerModule::~KFPlayerModule()
    {
        _kf_component = nullptr;
    }

    void KFPlayerModule::InitModule()
    {

    }

    void KFPlayerModule::BeforeRun()
    {
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFPlayerModule::OnClientConnectionWorld );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFPlayerModule::TransmitMessageToPlayer );
        __REGISTER_SHUTDOWN_FUNCTION__( &KFPlayerModule::OnDeployShutDownServer );
        _kf_route->RegisterTransmitFunction( this, &KFPlayerModule::TransmitMessageToPlayer );

        // 注册逻辑函数
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        _kf_component->SetEntityDataMask( __NEED_TO_SAVE__ | __DELETE_AND_SAVE__ );
        _kf_component->RegisterEntitySaveFunction( this, &KFPlayerModule::SavePlayer );

        _kf_component->RegisterEntityInitializeFunction( this, &KFPlayerModule::InitPlayer );
        _kf_component->RegisterEntityUninitializeFunction( this, &KFPlayerModule::UnInitPlayer );
        _kf_component->RegisterEntityRunFunction( this, &KFPlayerModule::RunPlayer );
        _kf_component->RegisterEntityAfterRunFunction( this, &KFPlayerModule::AfterRunPlayer );

        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFPlayerModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFPlayerModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFPlayerModule::SendUpdateDataToClient );
        _kf_component->RegisterShowRewardFunction( this, &KFPlayerModule::SendRewardAgentToClient );

        // 注册消息处理
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_SEX_REQ, &KFPlayerModule::HandleChangeSexReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_NAME_REQ, &KFPlayerModule::HandleChangeNameReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CREATE_ROLE_REQ, &KFPlayerModule::HandleCreateRoleReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_ACK, &KFPlayerModule::HandleCreateRoleAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_ACK, &KFPlayerModule::HandleSetPlayerNameAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ, &KFPlayerModule::HandleKickGamePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &KFPlayerModule::HandlePlayerDisconnectionReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ, &KFPlayerModule::HandleChangeIconReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ, &KFPlayerModule::HandleQueryPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_ACK, &KFPlayerModule::HandleQueryPlayerAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ, &KFPlayerModule::HandleRemoveDataReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ, &KFPlayerModule::HandleChangeIconBoxReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ, &KFPlayerModule::HandleChangeMottoReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUEST_REQ, &KFPlayerModule::HandleQueryGuestReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_ACK, &KFPlayerModule::HandleQueryGuestAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_TOAST_COUNT_REQ, &KFPlayerModule::HandleQueryToastCountReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_ACK, &KFPlayerModule::HandleQueryToastCountAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_SETTING_REQ, &KFPlayerModule::HandleQuerySettingReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_UPDATE_SETTING_REQ, &KFPlayerModule::HandleUpdateSettingReq );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFPlayerModule::OnceRun()
    {
        _kf_player_data = _kf_kernel->CreateObject( __KF_STRING__( player ) );
    }

    void KFPlayerModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();
        __UNREGISTER_SHUTDOWN_FUNCTION__();

        _kf_kernel->ReleaseObject( _kf_player_data );
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

        // 卸载消息处理
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_SEX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CREATE_ROLE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_GAME_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_DISCONNECTION_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_REMOVE_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_ICON_BOX_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MOTTO_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_GUEST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_TOAST_COUNT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_SETTING_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_UPDATE_SETTING_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFPlayerModule::LogPlayer( KFEntity* player, const char* format, ... )
    {
        char _buffer[ 4096 ] = "";

        va_list args;
        va_start( args, format );
        vsprintf( _buffer, format, args );
        va_end( args );

        auto name = player->GetName();
        auto playerid = player->GetKeyID();
        KFLogger::LogPlayer( KFLogger::Info, "[%s]\t[%u] %s", name, playerid, _buffer );
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

    void KFPlayerModule::AfterRunPlayer( KFEntity* kfentity )
    {
        // 更新属性
        //UpdatePlayerDataToClient( selfid, kfobject );
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

    void KFPlayerModule::SavePlayer( KFEntity* kfentity )
    {
        auto kfdata = kfentity->GetData();
        auto zoneid = KFUtility::CalcZoneId( kfdata->GetKeyID() );
        if ( !kfentity->IsInited() || !kfentity->IsNeedToSave() )
        {
            return;
        }

        KFMsg::S2SSavePlayerReq req;
        req.set_zoneid( zoneid );
        req.set_id( kfdata->GetKeyID() );
        _kf_kernel->SerializeToData( kfdata, req.mutable_data() );
        auto ok = _kf_data->SendMessageToData( zoneid, KFMsg::S2S_SAVE_PLAYER_REQ, &req );
        if ( ok )
        {
            kfentity->SetNeetToSave( false );
            //KFLogger::LogLogic( KFLogger::Debug, "[%s] player[%u:%s] save send ok!",
            //__FUNCTION__, zoneid, kfentity->GetKeyString() );
        }
        else
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u:%s] save send failed!",
                                __FUNCTION__, zoneid, kfentity->GetKeyString() );
        }
    }

    void KFPlayerModule::SendEnterMessageToWorld( KFEntity* player )
    {
        auto kfobject = player->GetData();
        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerEnterWorldReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_accountid( kfobject->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( accountid ) ) );
            _kf_game->SendMessageToWorld( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ, &req );
        }
    }

    void KFPlayerModule::SendLeaveMessageToWord( KFEntity* player )
    {
        auto kfobject = player->GetData();

        {
            // 发送消息到世界服务器
            KFMsg::S2SPlayerLeaveWorldReq req;
            req.set_playerid( player->GetKeyID() );
            req.set_accountid( kfobject->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( accountid ) ) );
            _kf_game->SendMessageToWorld( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ, &req );
        }
    }

    uint32 KFPlayerModule::GetPlayerCount()
    {
        return _kf_component->GetEntityCount();
    }

    KFEntity* KFPlayerModule::CreatePlayer( uint32 gateid, uint32 playerid, const KFMsg::PBObject* playerdata, const KFMsg::PBStrings* extenddata )
    {
        auto player = _kf_component->CreateEntity( playerid, playerdata );

        // 额外数据
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        for ( auto i = 0; i < extenddata->pbstring_size(); ++i )
        {
            auto pbdata = &extenddata->pbstring( i );
            kfbasic->SetValue< std::string >( pbdata->name(), pbdata->value() );
        }

        // 创建玩家
        OnEnterCreatePlayer( player, gateid, playerid );

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

    void KFPlayerModule::OnEnterCreatePlayer( KFEntity* player, uint32 gateid, uint32 playerid )
    {
        auto kfglobal = KFGlobal::Instance();

        auto kfobject = player->GetData();
        kfobject->SetValue( __KF_STRING__( gateid ), gateid );

        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );
        kfbasic->SetValue< uint32 >( __KF_STRING__( serverid ), kfglobal->_app_id );
        kfbasic->SetValue< uint32 >( __KF_STRING__( status ), KFMsg::StatusEnum::OnlineStatus );
        kfbasic->SetValue< uint64 >( __KF_STRING__( statustime ), kfglobal->_real_time );

        // 设置名字
        auto name = kfbasic->GetValue< std::string >( __KF_STRING__( name ) );
        auto asciiname = KFConvert::ToAscii( name );
        player->SetName( asciiname );

        // 判断新玩家
        auto basicid = kfbasic->GetValue< uint32 >( __KF_STRING__( id ) );
        if ( basicid != _invalid_int )
        {
            return;
        }

        kfbasic->SetValue< uint32 >( __KF_STRING__( id ), playerid );
        kfobject->SetValue < uint64 >( __KF_STRING__( birthday ), kfglobal->_real_time );
        for ( auto iter : _new_player_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }
    }

    KFEntity* KFPlayerModule::FindPlayer( uint32 playerid, const char* function, uint32 line )
    {
        return _kf_component->FindEntity( playerid, function, line );
    }

    bool KFPlayerModule::SendMessageToClient( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto player = FindPlayer( playerid, __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return false;
        }

        return SendMessageToClient( player, msgid, message );
    }

    bool KFPlayerModule::SendMessageToClient( uint32 playerid, uint32 msgid, const char* data, uint32 length )
    {
        auto player = FindPlayer( playerid, __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return false;
        }

        return SendMessageToClient( player, msgid, data, length );
    }

    bool KFPlayerModule::SendMessageToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfobject = player->GetData();
        auto playerid = static_cast< uint32 >( player->GetKeyID() );
        auto gateid = kfobject->GetValue< uint32 >( __KF_STRING__( gateid ) );

        return _kf_game->SendMessageToClient( gateid, playerid, msgid, message );
    }

    bool KFPlayerModule::SendMessageToClient( KFEntity* player, uint32 msgid, const char* data, uint32 length )
    {
        auto kfobject = player->GetData();
        auto playerid = static_cast< uint32 >( player->GetKeyID() );
        auto gateid = kfobject->GetValue< uint32 >( __KF_STRING__( gateid ) );

        return _kf_game->SendMessageToClient( gateid, playerid, msgid, data, length );
    }

    bool KFPlayerModule::SendMessageToClient( KFData* kfbasic, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto serverid = kfbasic->GetValue< uint32 >( __KF_STRING__( serverid ) );
        auto playerid = kfbasic->GetValue< uint32 >( __KF_STRING__( id ) );

        return _kf_route->SendMessageToRoute( serverid, playerid, msgid, message );
    }

    void KFPlayerModule::SendMessageToGroup( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message )
    {
        // 先发送给自己
        SendMessageToClient( player, msgid, message );

        auto kfobject = player->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( group ), __KF_STRING__( groupmember ) );

        auto kfmember = kfmemberrecord->FirstData();
        while ( kfmember != nullptr )
        {
            if ( kfmember->GetKeyID() != player->GetKeyID() )
            {
                auto kfbasic = kfmember->FindData( __KF_STRING__( basic ) );
                SendMessageToClient( kfbasic, msgid, message );
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

        _kf_game->SendMessageToWorld( KFMsg::S2S_GAME_SYNC_ONLINE_REQ, &req );
    }

    __KF_TRANSMIT_FUNCTION__( KFPlayerModule::TransmitMessageToPlayer )
    {
        auto playerid = __KF_DATA_ID__( kfguid );
        auto player = FindPlayer( playerid, __FUNCTION__, _invalid_int );
        if ( player == nullptr )
        {
            return true;
        }

        return SendMessageToClient( player, msgid, data, length );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandlePlayerDisconnectionReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerDisconnectionReq );

        KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] disconnection!", __FUNCTION__, kfmsg.playerid() );

        _kf_component->RemoveEntity( kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleKickGamePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickGamePlayerReq );

        auto playerid = kfmsg.playerid();
        KickPlayer( playerid, kfmsg.type(), __FUNCTION_LINE__ );
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

    void KFPlayerModule::KickPlayer( uint32 playerid, uint32 type, const char* function, uint32 line )
    {
        auto player = FindPlayer( playerid, function, line );
        if ( player == nullptr )
        {
            return;
        }

        KFLogger::LogLogin( KFLogger::Info, "[%s:%u] kick player[%u] by type[%u]!", function, line, playerid, type );

        // 发送消息到proxy
        KFMsg::S2SKickProxyPlayerReq req;
        req.set_playerid( playerid );
        req.set_type( type );
        SendMessageToClient( player, KFMsg::S2S_KICK_PROXY_PLAYER_REQ, &req );

        // 删除玩家
        _kf_component->RemoveEntity( playerid );
    }

    void KFPlayerModule::LogoutPlayer( uint32 playerid )
    {
        KFLogger::LogLogin( KFLogger::Info, "[%s] player[%u] logout!", __FUNCTION__, playerid );

        auto player = FindPlayer( playerid, __FUNCTION_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        for ( auto iter : _player_logout_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( player );
        }

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
            //KFLogger::LogLogic( KFLogger::Debug, temp.c_str() );
        }
#endif

        KFMsg::MsgSyncUpdateData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        SendMessageToClient( player, KFMsg::MSG_SYNC_UPDATE_DATA, &sync );
    }

    void KFPlayerModule::SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
            //KFLogger::LogLogic( KFLogger::Debug, temp.c_str() );
        }
#endif

        KFMsg::MsgSyncAddData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        SendMessageToClient( player, KFMsg::MSG_SYNC_ADD_DATA, &sync );
    }

    void KFPlayerModule::SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
            //KFLogger::LogLogic( KFLogger::Debug, temp.c_str() );
        }
#endif

        KFMsg::MsgSyncRemoveData sync;
        sync.mutable_pbdata()->CopyFrom( pbobect );
        SendMessageToClient( player, KFMsg::MSG_SYNC_REMOVE_DATA, &sync );
    }

    void KFPlayerModule::SendRewardAgentToClient( KFEntity* player, const std::string& reward, bool showclient, const char* function, uint32 line )
    {
        if ( showclient )
        {
            KFMsg::MsgShowRewardAgent show;
            show.set_reward( reward );
            SendMessageToClient( player, KFMsg::MSG_SHOW_REWARD_AGENT, &show );
        }

        LogPlayer( player, "[ %s:%u ] add agent [ %s ]!", function, line, reward.c_str() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleCreateRoleReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgCreateRoleReq );

        if ( kfmsg.name().empty() || kfmsg.model() == _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::NameEmpty );
        }

        // 如果名字已经存在了, 说明已经创建过角色
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto modleid = kfbasic->GetValue< uint32 >( __KF_STRING__( model ) );
        if ( modleid != _invalid_int )
        {
            return _kf_display->SendToClient( player, KFMsg::CreateRoleAlready );
        }

        KFMsg::S2SCreateRoleReq req;
        req.set_playerid( playerid );
        req.set_newname( kfmsg.name() );
        req.set_modleid( kfmsg.model() );
        auto ok = _kf_public->SendMessageToPublic( KFMsg::S2S_CREATE_ROLE_REQ, &req );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u:%s] create role to public failed!",
                                __FUNCTION__, playerid, kfmsg.name().c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleCreateRoleAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SCreateRoleAck );

        if ( kfmsg.result() == KFMsg::Success )
        {
            kfmsg.set_result( KFMsg::CreateRoleOK );

            // 设置模型
            player->UpdateData( __KF_STRING__( modelid ), KFOperateEnum::Set, kfmsg.modleid() );
            player->UpdateData( __KF_STRING__( model ), kfmsg.modleid(), __KF_STRING__( id ), KFOperateEnum::Set, kfmsg.modleid() );

            // 名字
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( name ), kfmsg.newname() );

            // 保存玩家
            SavePlayer( player );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.newname() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleChangeSexReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeSexReq );

        _kf_display->SendToClient( player, KFMsg::SexSetOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( sex ), KFOperateEnum::Set, kfmsg.sex() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleChangeNameReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeNameReq );

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

        KFMsg::S2SSetPlayerNameReq req;
        req.set_playerid( playerid );
        req.set_oldname( name );
        req.set_newname( kfmsg.name() );
        auto ok = _kf_public->SendMessageToPublic( KFMsg::S2S_SET_PLAYER_NAME_REQ, &req );
        if ( !ok )
        {
            KFLogger::LogLogic( KFLogger::Error, "[%s] player[%u:%s:%s] set name to public failed!",
                                __FUNCTION__, playerid, name.c_str(), kfmsg.name().c_str() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleSetPlayerNameAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SSetPlayerNameAck );

        if ( kfmsg.result() == KFMsg::Success )
        {
            kfmsg.set_result( KFMsg::NameSetOK );
            player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( name ), kfmsg.newname() );
        }

        _kf_display->SendToClient( player, kfmsg.result(), kfmsg.newname() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleChangeIconReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeIconReq );

        _kf_display->SendToClient( player, KFMsg::ChangeIconOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( icon ), kfmsg.icon() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleChangeMottoReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeMottoReq );

        _kf_display->SendToClient( player, KFMsg::ChangeMottoOK );
        player->UpdateData( __KF_STRING__( motto ), kfmsg.motto() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryPlayerReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryPlayerReq );

        //不能查询自己的数据，客户端本地可以获取到
        if ( playerid == kfmsg.playerid() )
        {
            return _kf_display->SendToClient( player, KFMsg::CanNotInquireSelf );
        }

        auto zoneid = KFUtility::CalcZoneId( kfmsg.playerid() );

        //查询玩家数据
        KFMsg::S2SQueryPlayerReq req;
        req.set_playerid( playerid );
        req.set_zoneid( zoneid );
        req.set_queryid( kfmsg.playerid() );
        _kf_data->SendMessageToData( zoneid, KFMsg::S2S_QUERY_PLAYER_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryPlayerAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryPlayerAck );

        KFMsg::MsgTellQueryPlayer ack;
        auto pbplayer = ack.mutable_player();

        //从数据库获取到的数据
        auto pbobject = &kfmsg.pbobject();
        if ( pbobject->key() != _invalid_int )
        {
            //格式化数据
            _kf_player_data->Reset();
            _kf_kernel->ParseFromProto( _kf_player_data, pbobject );
            _kf_kernel->SerializeToView( _kf_player_data, pbplayer );

            //更新访客信息
            auto kfobject = player->GetData();

            KFMsg::S2SUpdateGuestListReq req;
            req.set_playerid( pbobject->key() );
            req.set_guestid( kfobject->GetKeyID() );
            req.set_guesttime( KFGlobal::Instance()->_real_time );
            _kf_public->SendMessageToPublic( KFMsg::S2S_UPDATE_GUEST_LIST_REQ, &req );
        }

        SendMessageToClient( player, KFMsg::MSG_TELL_QUERY_PLAYER, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleRemoveDataReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgRemoveDataReq );

        player->UpdateData( kfmsg.dataname(), kfmsg.key(), __KF_STRING__( count ), KFOperateEnum::Dec, kfmsg.count() );

        auto strkey = __TO_STRING__( kfmsg.key() );
        KFLogger::LogLogic( KFLogger::Info, "[%s] remove data[%s:%s:%u] ok!",
                            __FUNCTION__, kfmsg.dataname().c_str(), strkey.c_str(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleChangeIconBoxReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgChangeIconBoxReq );

        _kf_display->SendToClient( player, KFMsg::ChangeIconBoxOK );
        player->UpdateData( __KF_STRING__( basic ), __KF_STRING__( iconbox ), kfmsg.iconbox() );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryGuestReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryGuestReq );

        //查询访客信息
        KFMsg::S2SQueryGuestReq req;
        req.set_playerid( playerid );
        req.set_queryid( kfmsg.playerid() );
        req.set_querytime( KFGlobal::Instance()->_real_time );
        _kf_public->SendMessageToPublic( KFMsg::S2S_QUERY_GUEST_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryGuestAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryGuestAck );

        KFMsg::MsgQueryGuestAck ack;
        ack.set_playerid( kfmsg.queryid() );
        ack.set_guestcount( kfmsg.guestcount() );
        ack.mutable_guestdata()->CopyFrom( kfmsg.guestdata() );
        SendMessageToClient( player, KFMsg::MSG_QUERY_GUEST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryToastCountReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryToastCountReq );

        KFMsg::S2SQueryToastCountReq req;
        req.set_selfplayerid( playerid );
        req.set_targetplayerid( kfmsg.playerid() );
        _kf_public->SendMessageToPublic( KFMsg::S2S_QUERY_TOAST_COUNT_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQueryToastCountAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryToastCountAck );

        KFMsg::MsgQueryToastCountAck ack;
        ack.set_playerid( kfmsg.targetplayerid() );
        ack.set_toastcount( kfmsg.toastcount() );
        SendMessageToClient( player, KFMsg::MSG_QUERY_TOAST_COUNT_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleQuerySettingReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryBasicReq );

        auto kfobject = player->GetData();
        auto kfsetting = kfobject->FindData( __KF_STRING__( setting ) );

        KFMsg::MsgQuerySettingAck ack;
        _kf_kernel->SerializeToData( kfsetting, ack.mutable_pbsetting() );
        SendMessageToClient( player, KFMsg::MSG_QUERY_SETTING_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPlayerModule::HandleUpdateSettingReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgUpdateSettingReq );

        auto kfobject = player->GetData();
        auto kfsetting = kfobject->FindData( __KF_STRING__( setting ) );

        for ( auto i = 0; i < kfmsg.strsetting_size(); ++i )
        {
            auto setting = kfmsg.strsetting( i );
            player->UpdateData( kfsetting, setting.name(), setting.value() );
        }

        for ( auto i = 0; i < kfmsg.intsetting_size(); ++i )
        {
            auto setting = kfmsg.intsetting( i );
            player->UpdateData( kfsetting, setting.name(), KFOperateEnum::Set, setting.value() );
        }
    }
}