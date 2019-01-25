#include "KFAuthState.h"
#include "KFRobot.h"

namespace KFrame
{
    void KFAuthState::EnterState( KFRobot* kfrobot )
    {
        // 启动定时器, 10秒认证一次
        kfrobot->StartTimer( _kf_robot_config->_state_rep_time );
    }

    void KFAuthState::LeaveState( KFRobot* kfrobot )
    {
        kfrobot->StopTimer();
    }

    void KFAuthState::CheckState( KFRobot* kfrobot )
    {
        if ( !kfrobot->_gate_ip.empty() && !kfrobot->_gate_port.empty() )
        {
            kfrobot->ChangeState( RobotStateEnum::LoginGame );
        }
    }

    void KFAuthState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        kfrobot->_connect_ing = false;
        // 发送认证消息
        KFJson sendjson;
        sendjson.SetValue( __KF_STRING__( channel ), _kf_robot_config->_verify_channel );
        sendjson.SetValue( __KF_STRING__( account ),  kfrobot->_account );
        auto url = _kf_robot_config->_auth_server_ip + __KF_STRING__( auth );
        auto authdata = _kf_http_client->StartSTHttpClient( url, sendjson );

        KFJson authjson( authdata );
        uint32 recode = authjson.GetUInt32( __KF_STRING__( retcode ) );
        kfrobot->_accountid = authjson.GetUInt32( __KF_STRING__( accountid ) );
        kfrobot->_token = authjson.GetString( __KF_STRING__( token ) );
        if ( KFMsg::Ok != recode )
        {
            __LOG_ERROR__( "url:[{}], accountid:{} code:{}", url.c_str(), kfrobot->_account, authjson.GetUInt32( __KF_STRING__( retcode ) ) );
            return;
        }

        auto serverlisttype = authjson.GetUInt32( __KF_STRING__( serverlisttype ) );

        switch ( serverlisttype )
        {
        case 1:
        {
            std::vector<std::string> matchkeys;
            matchkeys.push_back( __KF_STRING__( id ) );
            matchkeys.push_back( __KF_STRING__( type ) );
            matchkeys.push_back( __KF_STRING__( ip ) );
            matchkeys.push_back( __KF_STRING__( port ) );
            matchkeys.push_back( __KF_STRING__( name ) );
            std::map<std::string, std::string> des;
            int cursor = 0;
            while ( authjson.GetArray( __KF_STRING__( serverlist ), matchkeys, cursor, des ) )
            {
                if ( des[ __KF_STRING__( id ) ] == __TO_STRING__( _kf_robot_config->_server_channel ) )
                {
                    kfrobot->_gate_server_id = des[ __KF_STRING__( id ) ];
                    kfrobot->_gate_server_type = des[ __KF_STRING__( type ) ];
                    kfrobot->_gate_ip = des[ __KF_STRING__( ip ) ];
                    kfrobot->_gate_port = des[ __KF_STRING__( port ) ];
                    kfrobot->_gate_name = des[ __KF_STRING__( name ) ];
                    break;
                }
            };
        }
        break;
        case 2:
        {
            kfrobot->_gate_ip = authjson.GetString( __KF_STRING__( ip ) );
            kfrobot->_gate_port = authjson.GetUInt32( __KF_STRING__( port ) );
        }
        break;
        default:
            break;
        }

    }
}

