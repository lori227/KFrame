#include "KFSignModule.hpp"

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
        __REGISTER_RESET__( __STRING__( sign ), &KFSignModule::OnResetSigninData );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFSignModule, KFMsg::MSG_SEVEN_SIGN_REWARD_REQ, KFMsg::MsgSevenSignRewardReq, HandleReceiveSevenRewardReq );
    }

    void KFSignModule::ShutDown()
    {
        __UN_RESET__( __STRING__( sign ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SEVEN_SIGN_REWARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFSignModule::HandleReceiveSevenRewardReq, KFMsg::MsgSevenSignRewardReq )
    {
        __ROUTE_FIND_PLAYER__;

        auto day = player->Get< uint32 >( __STRING__( sevenday ) );
        if ( day < kfmsg->day() )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInNotDay );
        }

        auto kfsetting = KFSignConfig::Instance()->FindSetting( kfmsg->day() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInCanNotFind );
        }

        auto sevenflag = player->Get< uint32 >( __STRING__( sevenreward ) );
        auto flag = 1u << kfmsg->day();
        if ( KFUtility::HaveBitMask< uint32 >( sevenflag, flag ) )
        {
            return _kf_display->SendToClient( player, KFMsg::SignInRewardAlready );
        }

        // 设置标记
        player->UpdateData( __STRING__( sevenreward ), KFEnum::Or, kfmsg->day() );

        // 添加奖励
        player->AddElement( &kfsetting->_reward, _default_multiple, __STRING__( sign ), kfmsg->day(), __FUNC_LINE__ );

        // 额外的奖励
        if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, kfsetting->_probability ) )
        {
            player->AddElement( &kfsetting->_extend_reward, _default_multiple, __STRING__( sign ), kfmsg->day(), __FUNC_LINE__ );
        }

        _kf_display->SendToClient( player, KFMsg::SignInRewardOk );
    }

    __KF_RESET_FUNCTION__( KFSignModule::OnResetSigninData )
    {
        // 计算签到
        CalcSignDay( player );

        // 计算连续签到
        CalcContinuousSign( player, lastresettime, nowresettime );
    }

    void KFSignModule::CalcSignDay( KFEntity* player )
    {
        // 签到逻辑, 只有到前一天奖励领取了, 才算成功签到
        auto day = player->Get< uint32 >( __STRING__( sevenday ) );
        if ( day > 0u )
        {
            auto sevenflag = player->Get< uint32 >( __STRING__( sevenreward ) );
            auto flag = 1u << day;
            if ( !KFUtility::HaveBitMask< uint32 >( sevenflag, flag ) )
            {
                return;
            }
        }

        player->UpdateData( __STRING__( sevenday ), KFEnum::Add, 1u );
    }

    void KFSignModule::CalcContinuousSign( KFEntity* player, uint64 lastresettime, uint64 nowresettime )
    {
        auto kfsignintime = player->Find( __STRING__( signtime ) );

        // 判断连续签到
        auto lastsignintime = kfsignintime->Get();
        if ( lastsignintime == lastresettime )
        {
            player->UpdateData( __STRING__( continuoussign ), KFEnum::Add, 1u );
        }
        else
        {
            player->UpdateData( __STRING__( continuoussign ), KFEnum::Set, 1u );
        }

        // 更新本次签到
        player->UpdateData( kfsignintime, KFEnum::Set, nowresettime );
    }
}