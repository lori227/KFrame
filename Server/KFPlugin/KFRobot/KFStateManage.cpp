#include "KFStateManage.h"
#include "KFIdleState.h"
#include "KFAuthState.h"
#include "KFLoginVerifyState.h"
#include "KFLoginGameState.h"
#include "KFCreateRoleState.h"
#include "KFQueryMailNumState.h"
#include "KFQueryMailState.h"
#include "KFViewMailState.h"
#include "KFReciveMailRewardState.h"
#include "KFDelMailState.h"
#include "KFEnterChatState.h"
#include "KFSendChatState.h"
#include "KFLeaveChatState.h"
#include "KFBuyStoreState.h"
#include "KFGiveStoreState.h"
#include "KFAddFriendState.h"
#include "KFDelFriendState.h"
#include "KFSetRefuseFirendState.h"
#include "KFQueryBasicState.h"
#include "KFChangeNameState.h"
#include "KFChangeSexState.h"
#include "KFSendFriendChatState.h"
#include "KFChangeIconState.h"
#include "KFChangeIconBoxState.h"
#include "KFChangeMottoState.h"
#include "KFChangeModelState.h"
#include "KFChangeClothesState.h"
#include "KFDressClothesState.h"
#include "KFQueryGuestState.h"
#include "KFRecvActivityRewardState.h"
#include "KFRecvTaskRewardState.h"
#include "KFRecvAchieveRewardState.h"
#include "KFRecvGiftRewardState.h"
#include "KFToastState.h"
#include "KFInviteTeamState.h"
#include "KFAgreeInviteTeamState.h"
#include "KFRefuseInviteTeamState.h"
#include "KFRefuseMinuInviteTeamState.h"
#include "KFLeaveTeamState.h"
#include "KFApplyGroupState.h"
#include "KFAgreeApplyTeamState.h"
#include "KFRefuseApplyTeamState.h"
#include "KFKickGroupState.h"
#include "KFSingleMatchState.h"
#include "KFCancleMatchState.h"
#include "KFQueryPlayerState.h"
#include "KFStoreInfoState.h"
#include "KFStoreVersionState.h"
#include "KFDoubleMatchGpState.h"
#include "KFDoubleMatchState.h"
#include "KFPentaMatchGpState.h"
#include "KFPentaMatchState.h"
#include "KFAddDiamondState.h"
#include "KFAddMoneyState.h"
#include "KFAddWishOrderState.h"
#include "KFDelWishOrderState.h"
#include "KFModifyWishOrderState.h"
#include "KFPauseState.h"
#include "KFQueryWholeRankState.h"
#include "KFQueryFriendRankState.h"

namespace KFrame
{
    void KFStateManage::Initialize()
    {
        // 在这里添加所有状态信息
        AddState( RobotStateEnum::IdleState, new KFIdleState() );
        AddState( RobotStateEnum::AuthState, new KFAuthState() );
        AddState( RobotStateEnum::LoginVerify, new KFLoginVerifyState() );
        AddState( RobotStateEnum::LoginGame, new KFLoginGameState() );
        AddState( RobotStateEnum::CreateRole, new KFCreateRoleState() );

        // 邮件
        AddState( RobotStateEnum::QueryMailNum, new KFQueryMailNumState() );
        AddState( RobotStateEnum::QueryMail, new KFQueryMailState() );
        AddState( RobotStateEnum::ViewMail, new KFViewMailState() );
        AddState( RobotStateEnum::ReciveMailReward, new KFReciveMailRewardState() );
        AddState( RobotStateEnum::DelMail, new KFDelMailState() );

        // 聊天
        AddState( RobotStateEnum::EnterChat, new KFEnterChatState() );
        AddState( RobotStateEnum::SendChat, new KFSendChatState() );
        AddState( RobotStateEnum::LeaveChat, new KFLeaveChatState() );
        AddState( RobotStateEnum::SendFriendChat, new KFSendFriendChatState() );

        // 商城
        AddState( RobotStateEnum::BuyStore, new KFBuyStoreState() );
        AddState( RobotStateEnum::GiveStore, new KFGiveStoreState() );

        // 好友
        AddState( RobotStateEnum::AddFriend, new KFAddFriendState() );
        AddState( RobotStateEnum::DelFriend, new KFDelFriendState() );
        AddState( RobotStateEnum::RefuseFriend, new KFSetRefuseFirendState() );

        // 玩家状态/行为
        AddState( RobotStateEnum::QueryBasic, new KFQueryBasicState() );
        AddState( RobotStateEnum::ChangeName, new KFChangeNameState() );
        AddState( RobotStateEnum::ChangeSex, new KFChangeSexState() );
        AddState( RobotStateEnum::ChangeIcon, new KFChangeIconState() );
        AddState( RobotStateEnum::ChangeIconBox, new KFChangeIconBoxState() );
        AddState( RobotStateEnum::ChangeMotto, new KFChangeMottoState() );
        AddState( RobotStateEnum::ChangeModel, new KFChangeModelState() );
        AddState( RobotStateEnum::ChangeClothes, new KFChangeClothesState() );
        AddState( RobotStateEnum::DressClothes, new KFDressClothesState() );
        AddState( RobotStateEnum::QueryGuest, new KFQueryGuestState() );
        AddState( RobotStateEnum::Toask, new KFToastState() );

        // 奖励相关
        AddState( RobotStateEnum::RecvActivityReward, new KFRecvActivityRewardState() );
        AddState( RobotStateEnum::RecvTaskReward, new KFRecvTaskRewardState() );
        AddState( RobotStateEnum::RecvAchieveReward, new KFRecvAchieveRewardState() );
        AddState( RobotStateEnum::RecvGiftReward, new KFRecvGiftRewardState() );

        // 组队相关
        AddState( RobotStateEnum::InviteTeam, new KFInviteTeamState() );
        AddState( RobotStateEnum::AgreeInviteTeam, new KFAgreeInviteTeamState() );
        AddState( RobotStateEnum::RefuseInviteTeam, new KFRefuseInviteTeamState() );
        AddState( RobotStateEnum::RefuseMinuInviteTeam, new KFRefuseMinuInviteTeamState() );
        AddState( RobotStateEnum::LeaveTeam, new KFLeaveTeamState() );
        AddState( RobotStateEnum::ApplyGroup, new KFApplyGroupState() );
        AddState( RobotStateEnum::AgreeApplyTeam, new KFAgreeApplyTeamState() );
        AddState( RobotStateEnum::RefuseApplyTeam, new KFRefuseApplyTeamState() );
        AddState( RobotStateEnum::KickGroup, new KFKickGroupState() );

        // 匹配
        AddState( RobotStateEnum::SingleMatch, new KFSingleMatchState() );
        AddState( RobotStateEnum::CancelMatch, new KFCancleMatchState() );
        AddState( RobotStateEnum::DoubleMatchGroup, new KFDoubleMatchGpState() );
        AddState( RobotStateEnum::DoubleMatch, new KFDoubleMatchState() );
        AddState( RobotStateEnum::PentaMatchGroup, new KFPentaMatchGpState() );
        AddState( RobotStateEnum::PentaMatch, new KFPentaMatchState() );

        AddState( RobotStateEnum::QueryPlayer, new KFQueryPlayerState() );

        // 商城热更
        AddState( RobotStateEnum::QueryStoreVersion, new KFStoreVersionState() );
        AddState( RobotStateEnum::QueryStoreInfo, new KFStoreInfoState() );

        // 排行榜
        AddState( RobotStateEnum::QueryWholeRankList, new KFQueryWholeRankState() );
        AddState( RobotStateEnum::QueryFriendRankList, new KFQueryFriendRankState() );


        // GM加钱
        AddState( RobotStateEnum::GMAddMoney, new KFAddMoneyState() );
        AddState( RobotStateEnum::GMAddDiamond, new KFAddDiamondState() );

        AddState( RobotStateEnum::AddWishOrder, new KFAddWishOrderState() );
        AddState( RobotStateEnum::DelWishOrder, new KFDelWishOrderState() );
        AddState( RobotStateEnum::ModifyWishOrder, new KFModifyWishOrderState() );

        AddState( RobotStateEnum::PauseState, new KFPauseState() );
    }

    void KFStateManage::AddState( uint32 state, KFState* kfstate )
    {
        _state_list[ state ] = kfstate;
        kfstate->_state = state;
    }

    KFState* KFStateManage::FindState( uint32 state )
    {
        auto iter = _state_list.find( state );
        if ( iter == _state_list.end() )
        {
            return nullptr;
        }

        return iter->second;
    }
}
