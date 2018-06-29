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
            kfrobot->ChangeState( RobotStateEnum::LoginVerify );
        }
    }

    void KFAuthState::RunState( KFRobot* kfrobot )
    {
        if ( !kfrobot->DoneTimer() )
        {
            return;
        }

        // 发送认证消息
        KFJson sendjson;
        sendjson.SetValue( KFField::_channel, _kf_robot_config->_verify_channel );
        sendjson.SetValue( KFField::_account,  kfrobot->_account );
        auto url = _kf_robot_config->_auth_server_ip + KFField::_auth;
        auto authdata = _kf_http_client->StartSTHttpClient( url, sendjson, true );

        KFJson authjson( authdata );
        uint32 recode = authjson.GetUInt32( KFField::_ret_code );
        kfrobot->_accountid = authjson.GetUInt32( KFField::_account_id );
        kfrobot->_token = authjson.GetString( KFField::_token );
        if ( KFMsg::Success != recode )
        {
            KFLogger::LogScript( KFLogger::Error, "url:[%s], accountid:%s code:%d", url.c_str(), kfrobot->_account, authjson.GetUInt32( KFField::_ret_code ) );
            return;
        }

        auto serverlisttype = authjson.GetUInt32( KFField::_server_list_type );

        switch ( serverlisttype )
        {
        case 1:
        {
            std::vector<std::string> matchkeys;
            matchkeys.push_back( KFField::_id );
            matchkeys.push_back( KFField::_type );
            matchkeys.push_back( KFField::_ip );
            matchkeys.push_back( KFField::_port );
            matchkeys.push_back( KFField::_name );
            std::map<std::string, std::string> des;
            int cursor = 0;
            while ( authjson.GetArray( KFField::_server_list, matchkeys, cursor, des ) )
            {
                if ( des[ KFField::_id ] == __KF_STRING__( _kf_robot_config->_server_channel ) )
                {
                    kfrobot->_gate_server_id = des[ KFField::_id ];
                    kfrobot->_gate_server_type = des[ KFField::_type ];
                    kfrobot->_gate_ip = des[ KFField::_ip ];
                    kfrobot->_gate_port = des[ KFField::_port ];
                    kfrobot->_gate_name = des[ KFField::_name ];
                    break;
                }
            };
        }
        break;
        case 2:
        {
            kfrobot->_gate_ip = authjson.GetString( KFField::_ip );
            kfrobot->_gate_port = authjson.GetUInt32( KFField::_port );
        }
        break;
        default:
            break;
        }

    }
}

