#include "KFRobotModule.hpp"
#include "KFStateManage.hpp"

namespace KFrame
{
    void KFRobotModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_robot_config, true );
    }

#define __REGISTER_ROBOT_MESSAGE__( msgid, handle ) \
    {\
        KFMessageFunction function = std::bind( handle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
        auto kffunction = _message_function.Create( msgid );\
        kffunction->_function = function;\
    }

    void KFRobotModule::BeforeRun()
    {
        _kf_plugin_manage->RegisterCommandFunction( __KF_STRING__( robot ), this, &KFRobotModule::ProcessRobotCommand );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_RESULT_DISPLAY, &KFRobotModule::HandleResultDisplay );
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_LOGIN_ACK, &KFRobotModule::HandleLoginAck );
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_SHOW_ELEMENT, &KFRobotModule::HandleShowElement );
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_SYNC_ADD_DATA, &KFRobotModule::HandleSyncAddData );
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_SYNC_UPDATE_DATA, &KFRobotModule::HandlesyncUpdateData );
        __REGISTER_ROBOT_MESSAGE__( KFMsg::MSG_SYNC_REMOVE_DATA, &KFRobotModule::HandlesyncRemoveData );
    }

    void KFRobotModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __KF_REMOVE_CONFIG__( _kf_robot_config );
        _kf_plugin_manage->UnRegisterCommandFunction( __KF_STRING__( robot ) );
    }

    void KFRobotModule::OnceRun()
    {
        _kf_state_manage->Initialize();
        __REGISTER_LOOP_TIMER__( 0, _kf_robot_config->_login_interval_time, 1000, &KFRobotModule::OnTimerCreateRobot );

        _net_client = new KFNetClientEngine();
        _net_client->InitEngine( 200, KFMessageEnum::Client );
        _net_client->BindNetFunction( this, &KFRobotModule::HandleNetMessage );
        _net_client->BindConnectFunction( this, &KFRobotModule::OnClientConnectGate );
        _net_client->BindDisconnectFunction( this, &KFRobotModule::OnClientLostGate );
        _net_client->BindShutdownFunction( this, &KFRobotModule::OnClientShutdownGate );
    }

    void KFRobotModule::Run()
    {
        for ( auto& iter : _robots._objects )
        {
            iter.second->Run();
        }

        _net_client->RunEngine( KFGlobal::Instance()->_game_time );
    }

    void KFRobotModule::HandleNetMessage( const Route& route, uint32 msgid, const char* data, uint32 length )
    {
        if ( msgid == 0u )
        {
            return;
        }

        auto kffunction = _message_function.Find( msgid );
        if ( kffunction == nullptr )
        {
            return __LOG_ERROR__( "msgid=[{}] no function!", msgid );
        }

        kffunction->_function( route, data, length );
    }

    __KF_NET_EVENT_FUNCTION__( KFRobotModule::OnClientConnectGate )
    {
        auto robot = _robots.Find( netdata->_id );
        if ( robot == nullptr )
        {
            return;
        }

        robot->_client_id = netdata->_id;
    }

    __KF_NET_EVENT_FUNCTION__( KFRobotModule::OnClientLostGate )
    {
        auto robot = _robots.Find( netdata->_id );
        if ( robot == nullptr )
        {
            return;
        }

        _net_client->CloseClient( netdata->_id, __FUNC_LINE__ );
    }

    __KF_NET_EVENT_FUNCTION__( KFRobotModule::OnClientShutdownGate )
    {
        auto robot = _robots.Find( netdata->_id );
        if ( robot == nullptr )
        {
            return;
        }

        robot->ChangeState( KFrame::StateAuth );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFRobotModule::OnTimerCreateRobot )
    {
        if ( _robot_serial >= _kf_robot_config->_total_robot_count )
        {
            return;
        }

        ++_robot_serial;
        auto robotid = KFGlobal::Instance()->_app_id->GetWorkId() * 1000 + _robot_serial;

        auto robot = _robots.Create( robotid );
        robot->Init( robotid, _net_client );
    }

    void KFRobotModule::ProcessRobotCommand( const VectorString& params )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto robotid = KFUtility::ToValue<uint64 >( params[ 0 ] );

        VectorString paramlist;
        paramlist.assign( params.begin() + 1, params.end() );

        if ( robotid == 0u )
        {
            for ( auto& iter : _robots._objects )
            {
                auto robot = iter.second;
                robot->ProcessCommand( paramlist );
            }
        }
        else
        {
            auto robot = _robots.Find( robotid );
            if ( robot != nullptr )
            {
                robot->ProcessCommand( paramlist );
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
#define __ROBOT_PROTO_PARSE__( msgtype ) \
    msgtype kfmsg;\
    if ( !KFProto::Parse( &kfmsg, data, length ) )\
    {\
        return;\
    }\
    auto robot = _robots.Find( __ROUTE_SEND_ID__ );\
    if ( robot == nullptr )\
    {\
        return;\
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleResultDisplay )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgResultDisplay );

        __LOG_ERROR__( "robot=[{}:{}], error=[{}]!", robot->_id, robot->_account_id, kfmsg.result() );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleLoginAck )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgLoginAck );

        robot->_player_id = kfmsg.playerid();
        robot->_player = robot->_kf_component->CreateEntity( kfmsg.playerid(), &kfmsg.playerdata() );

        __LOG_INFO__( "robot=[{}|{}:{}] enter game ok!", robot->_id, robot->_account_id, robot->_player_id );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleShowElement )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgShowElement );

        __LOG_INFO__( "robot=[{}|{}:{}], reward={}", robot->_id, robot->_account_id, robot->_player_id, kfmsg.element() );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleSyncAddData )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgSyncAddData );

        if ( _kf_robot_config->_open_data_log )
        {
            auto temp = kfmsg.DebugString();
            __LOG_DEBUG__( "adddata = {}", temp );
        }

        auto kfobject = robot->_player->GetData();
        robot->SyncAddObject( kfobject, &kfmsg.pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandlesyncUpdateData )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgSyncUpdateData );

        if ( _kf_robot_config->_open_data_log )
        {
            auto temp = kfmsg.DebugString();
            __LOG_DEBUG__( "updatedata = {}", temp );
        }

        auto kfobject = robot->_player->GetData();
        robot->SyncUpdateData( kfobject, &kfmsg.pbdata() );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandlesyncRemoveData )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgSyncRemoveData );

        if ( _kf_robot_config->_open_data_log )
        {
            auto temp = kfmsg.DebugString();
            __LOG_DEBUG__( "updatedata = {}", temp );
        }

        auto kfobject = robot->_player->GetData();
        robot->SyncRemoveData( kfobject, &kfmsg.pbdata() );
    }
}
