#include "KFStateLogin.hpp"
#include "KFRobotModule.hpp"

namespace KFrame
{
    void KFStateLogin::EnterState( KFFsm* fsm, KFRobot* robot )
    {
        // 连接服务器
        robot->StartConnectGate();
    }

    bool KFStateLogin::CheckState( KFFsm* fsm, KFRobot* robot )
    {
        // 还没有连接上, 不执行逻辑
        if ( robot->_client_id == 0u )
        {
            return false;
        }

        return true;
    }

    void KFStateLogin::RunState( KFFsm* fsm, KFRobot* robot )
    {
        __LOG_DEBUG__( "robot[{}|{}] start login!", robot->_id, robot->_account_id );

        if ( robot->_player_id != 0u )
        {
            robot->_player_id = 0u;
            robot->_player = nullptr;
            robot->_kf_component->RemoveEntity( robot->_player_id );
        }

        // 发送登陆消息
        KFMsg::MsgLoginReq req;
        req.set_version( "0.0.0.0" );
        req.set_token( robot->_login_token );
        req.set_accountid( robot->_account_id );
        auto ok = robot->SendNetMessage( KFMsg::MSG_LOGIN_REQ, &req );
        if ( ok )
        {
            fsm->ChangeState( KFrame::StateWait );
        }
    }
}