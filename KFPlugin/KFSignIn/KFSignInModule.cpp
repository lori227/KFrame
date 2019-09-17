#include "KFSignInModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFSignInModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_SEVEN_SIGNIN_REWARD_REQ, &KFSignInModule::HandleReceiveSevenRewardReq );
    }

    void KFSignInModule::ShutDown()
    {
        __UN_MESSAGE__( KFMsg::MSG_SEVEN_SIGNIN_REWARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFSignInModule::HandleReceiveSevenRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSevenSignInRewardReq );

        auto kfobject = player->GetData();
        auto day = kfobject->GetValue< uint32 >( __KF_STRING__( sevenday ) );
        if ( day < kfmsg.day() )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInNotDay );
        }

        auto kfsetting = KFSignInConfig::Instance()->FindSetting( kfmsg.day() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInCanNotFind );
        }

        auto sevenflag = kfobject->GetValue< uint32 >( __KF_STRING__( sevenreward ) );
        auto flag = 1u << kfmsg.day();
        if ( KFUtility::HaveBitMask< uint32 >( sevenflag, flag ) )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInRewardAlready );
        }

        // 设置标记
        player->UpdateData( __KF_STRING__( sevenreward ), KFEnum::ABit, kfmsg.day() );

        // 添加奖励
        player->AddElement( &kfsetting->_reward, true, __FUNC_LINE__ );

        // 额外的奖励
        if ( kfsetting->_probability > 0u )
        {
            auto rand = KFGlobal::Instance()->RandRatio( KFRandEnum::TenThousand );
            if ( rand < kfsetting->_probability )
            {
                player->AddElement( &kfsetting->_extend, true, __FUNC_LINE__ );
            }
        }

        _kf_display->SendToClient( player, KFMsg::SignInRewardOk );
    }
}