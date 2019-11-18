#include "KFRobotModule.hpp"
#include "KFStateManage.hpp"

namespace KFrame
{
#define __REGISTER_ROBOT_MESSAGE__( msgid, handle ) \
    {\
        KFMessageFunction function = std::bind( handle, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3 );\
        auto kffunction = _message_function.Create( msgid );\
        kffunction->_function = function;\
    }


#define __REGISTER_ROBOT_COMMAND__( command, handle ) \
    {\
        KFRobotCmdFunction function = std::bind( handle, this, std::placeholders::_1, std::placeholders::_2 );\
        auto kffunction = _cmd_function.Create( command );\
        kffunction->_function = function;\
    }

    void KFRobotModule::BeforeRun()
    {
        _kf_plugin_manage->RegisterCommandFunction( __STRING__( robot ), this, &KFRobotModule::ProcessRobotCommand );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_ROBOT_COMMAND__( "setname", &KFRobotModule::CommandSetName );
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
        __UN_TIMER_0__();
        _kf_plugin_manage->UnRegisterCommandFunction( __STRING__( robot ) );
    }

    void KFRobotModule::PrepareRun()
    {
        _kf_state_manage->Initialize();
        __LOOP_TIMER_0__( KFRobotConfig::Instance()->_login_interval_time, 1000, &KFRobotModule::OnTimerCreateRobot );

        _net_client = new KFNetClientEngine();
        _net_client->InitEngine( 200, KFMessageEnum::Client, KFRobotConfig::Instance()->_compress_level );
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
        if ( _robot_serial >= KFRobotConfig::Instance()->_total_robot_count )
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
                ProcessRobotCommand( robot, paramlist );
            }
        }
        else
        {
            auto robot = _robots.Find( robotid );
            if ( robot != nullptr )
            {
                ProcessRobotCommand( robot, paramlist );
            }
        }
    }

    void KFRobotModule::ProcessRobotCommand( KFRobot* robot, const VectorString& params )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        auto& command = params[ 0 ];
        auto kffunction = _cmd_function.Find( command );
        if ( kffunction != nullptr )
        {
            VectorString paramlist;
            paramlist.assign( params.begin() + 1, params.end() );
            kffunction->_function( robot, paramlist );
        }
        else
        {
            if ( params.size() < 2 )
            {
                return;
            }

            auto& strdata = params[ 1 ];

            KFMsg::MsgCommandReq req;
            req.set_command( command );
            req.add_params( strdata );
            robot->SendNetMessage( KFMsg::MSG_COMMAND_REQ, &req );
        }
    }

    void KFRobotModule::CommandSetName( KFRobot* robot, const VectorString& params )
    {
        if ( params.size() < 1 )
        {
            return;
        }

        KFMsg::MsgSetNameReq req;
        req.set_name( params[ 0 ] );
        robot->SendNetMessage( KFMsg::MSG_SET_NAME_REQ, &req );
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

        if ( KFRobotConfig::Instance()->_open_data_log )
        {
            auto temp = kfmsg.playerdata().DebugString();
            __LOG_DEBUG__( "playerdata = {}", temp );
        }
        __LOG_INFO__( "robot=[{}|{}:{}] enter game ok!", robot->_id, robot->_account_id, robot->_player_id );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleShowElement )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgShowElement );


        __LOG_INFO__( "robot=[{}|{}:{}], reward={}", robot->_id, robot->_account_id, robot->_player_id, kfmsg.element().DebugString() );
    }

    __KF_MESSAGE_FUNCTION__( KFRobotModule::HandleSyncAddData )
    {
        __ROBOT_PROTO_PARSE__( KFMsg::MsgSyncAddData );

        if ( KFRobotConfig::Instance()->_open_data_log )
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

        if ( KFRobotConfig::Instance()->_open_data_log )
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

        if ( KFRobotConfig::Instance()->_open_data_log )
        {
            auto temp = kfmsg.DebugString();
            __LOG_DEBUG__( "updatedata = {}", temp );
        }

        auto kfobject = robot->_player->GetData();
        robot->SyncRemoveData( kfobject, &kfmsg.pbdata() );
    }
}
