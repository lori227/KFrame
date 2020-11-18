#include "KFSignModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    enum KFSignEnum
    {
        SevenDay = 1,		// 7天签到
    };
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFSignModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_SEVEN_SIGN_REWARD_REQ, &KFSignModule::HandleReceiveSevenRewardReq );
    }

    void KFSignModule::ShutDown()
    {
        __UN_MESSAGE__( KFMsg::MSG_SEVEN_SIGN_REWARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFSignModule::HandleReceiveSevenRewardReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSevenSignRewardReq );

        auto day = player->Get< uint32 >( __STRING__( sevenday ) );
        if ( day < kfmsg.day() )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInNotDay );
        }

        auto kfsetting = KFSignConfig::Instance()->FindSetting( kfmsg.day() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInCanNotFind );
        }

        auto sevenflag = player->Get< uint32 >( __STRING__( sevenreward ) );
        auto flag = 1u << kfmsg.day();
        if ( KFUtility::HaveBitMask< uint32 >( sevenflag, flag ) )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInRewardAlready );
        }

        // 设置标记
        player->UpdateData( __STRING__( sevenreward ), KFEnum::Or, kfmsg.day() );

        // 添加奖励
        player->AddElement( &kfsetting->_reward, _default_multiple, __STRING__( sign ), kfmsg.day(), __FUNC_LINE__ );

        // 额外的奖励
        if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, kfsetting->_probability ) )
        {
            player->AddElement( &kfsetting->_extend_reward, _default_multiple, __STRING__( sign ), kfmsg.day(), __FUNC_LINE__ );
        }

        _kf_display->SendToClient( player, KFMsg::SignInRewardOk );
    }
}