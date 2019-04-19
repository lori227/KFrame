#include "KFStateAuth.hpp"
#include "KFRobotModule.hpp"

namespace KFrame
{
    void KFStateAuth::EnterState( KFFsm* fsm, KFRobot* robot )
    {
        robot->_login_gate_ip.clear();
        robot->_login_gate_port = 0u;

        // 10验证一次
        fsm->StartTimer( 10000, true );
    }

    void KFStateAuth::RunState( KFFsm* fsm, KFRobot* robot )
    {
        __JSON_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( channel ), 1 );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( account ), robot->_account );

        auto strresult = _kf_http_client->STGet( _kf_robot_config->_auth_address, kfjson );

        __JSON_PARSE_STRING__( kfresult, strresult );
        auto retcode = __JSON_GET_UINT32__( kfresult, __KF_STRING__( retcode ) );
        if ( retcode != KFMsg::Ok )
        {
            return __LOG_ERROR__( "robot=[{}] auth failed=[{}]", robot->_account, retcode );
        }

        robot->_login_token = __JSON_GET_STRING__( kfresult, __KF_STRING__( token ) );
        robot->_account_id = __JSON_GET_UINT64__( kfresult, __KF_STRING__( accountid ) );

        auto& zonedata = __JSON_GET_OBJECT__( kfresult, __KF_STRING__( zone ) );
        auto ip = __JSON_GET_STRING__( zonedata, __KF_STRING__( ip ) );
        auto port = __JSON_GET_UINT32__( zonedata, __KF_STRING__( port ) );
        if ( ip.empty() || port == 0u )
        {
            return __LOG_ERROR__( "robot=[{}] ipport empty!", robot->_account );
        }

        robot->_login_gate_ip = ip;
        robot->_login_gate_port = port;

        // 切换到登陆状态
        fsm->ChangeState( KFrame::StateLogin );
    }
}