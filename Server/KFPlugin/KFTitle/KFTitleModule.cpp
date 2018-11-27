#include "KFTitleModule.h"

namespace KFrame
{
    void KFTitleModule::BeforeRun()
    {
        //__REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ, &KFTaskModule::HandleTitleChangeReq );
    }

    void KFTitleModule::BeforeShut()
    {
        //__UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_TASK_REWARD_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTitleModule::HandleTitleChangeReq )
    {
        //__CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveTaskRewardReq );

        //auto result = ReceiveTaskReward( player, kfmsg.taskid() );
        //_kf_display->SendToClient( player, result, kfmsg.taskid() );
    }
}