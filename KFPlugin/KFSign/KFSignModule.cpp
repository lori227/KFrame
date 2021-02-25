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
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SEVEN_SIGN_REWARD_REQ, &KFSignModule::HandleReceiveSevenRewardReq );
    }

    void KFSignModule::ShutDown()
    {
        __UN_RESET__( __STRING__( sign ) );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_SEVEN_SIGN_REWARD_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFSignModule::HandleReceiveSevenRewardReq, KFMsg::MsgSevenSignRewardReq )
    {
        auto day = entity->Get<uint32>( __STRING__( sevenday ) );
        if ( day < kfmsg->day() )
        {
            return _kf_display->SendToClient( entity, KFMsg::SignInNotDay );
        }

        auto setting = KFSignConfig::Instance()->FindSetting( kfmsg->day() );
        if ( setting == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::SignInCanNotFind );
        }

        auto seven_flag = entity->Get<uint32>( __STRING__( sevenreward ) );
        auto flag = 1u << kfmsg->day();
        if ( KFUtility::HaveBitMask<uint32>( seven_flag, flag ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::SignInRewardAlready );
        }

        // 设置标记
        entity->UpdateData( __STRING__( sevenreward ), KFEnum::Or, kfmsg->day() );

        // 添加奖励
        entity->AddElement( &setting->_reward, _default_multiple, __STRING__( sign ), kfmsg->day(), __FUNC_LINE__ );

        // 额外的奖励
        if ( KFGlobal::Instance()->RandCheck( KFRandEnum::TenThousand, setting->_probability ) )
        {
            entity->AddElement( &setting->_extend_reward, _default_multiple, __STRING__( sign ), kfmsg->day(), __FUNC_LINE__ );
        }

        _kf_display->SendToClient( entity, KFMsg::SignInRewardOk );
    }

    __KF_RESET_FUNCTION__( KFSignModule::OnResetSigninData )
    {
        // 计算签到
        CalcSignDay( player );

        // 计算连续签到
        CalcContinuousSign( player, last_reset_time, now_reset_time );
    }

    void KFSignModule::CalcSignDay( EntityPtr player )
    {
        // 签到逻辑, 只有到前一天奖励领取了, 才算成功签到
        auto day = player->Get<uint32>( __STRING__( sevenday ) );
        if ( day > 0u )
        {
            auto seven_flag = player->Get<uint32>( __STRING__( sevenreward ) );
            auto flag = 1u << day;
            if ( !KFUtility::HaveBitMask<uint32>( seven_flag, flag ) )
            {
                return;
            }
        }

        player->UpdateData( __STRING__( sevenday ), KFEnum::Add, 1u );
    }

    void KFSignModule::CalcContinuousSign( EntityPtr player, uint64 last_reset_time, uint64 now_reset_time )
    {
        auto sign_time = player->Find( __STRING__( signtime ) );

        // 判断连续签到
        auto last_sign_time = sign_time->Get();
        if ( last_sign_time == last_reset_time )
        {
            player->UpdateData( __STRING__( continuoussign ), KFEnum::Add, 1u );
        }
        else
        {
            player->UpdateData( __STRING__( continuoussign ), KFEnum::Set, 1u );
        }

        // 更新本次签到
        player->UpdateData( sign_time, KFEnum::Set, now_reset_time );
    }
}