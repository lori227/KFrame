#ifndef __KF_RANK_CLIENT_MODULE_H__
#define __KF_RANK_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    帮派数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-12
************************************************************************/

#include "KFRankClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFConfig/KFRankConfig.hpp"


namespace KFrame
{
    class KFRankClientModule : public KFRankClientInterface
    {
    public:
        KFRankClientModule() = default;
        ~KFRankClientModule() = default;

        // 刷新
        virtual void BeforeRun();
        virtual void AfterRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 属性更新回调
        __KF_UPDATE_DATA_FUNCTION__( OnDataUpdateCallBack );

    protected:
        // 请求查询全服排行榜
        __KF_MESSAGE_FUNCTION__( HandleQueryRankListReq, KFMsg::MsgQueryRankListReq );

        // 请求好友排行榜
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendRankListReq, KFMsg::MsgQueryFriendRankListReq );

    protected:
        // 更新排行榜数据
        void UpdateRankDataToShard( EntityPtr player, const KFRankSetting* setting );

        // 计算排行榜的打榜分值
        uint64 CalcRankDataScore( EntityPtr player, const KFRankSetting* setting );

        // 计算zoneid
        uint32 CalcRankZoneId( uint64 player_id, const KFRankSetting* setting );

    private:
        // 玩家组件
        std::shared_ptr<KFComponent> _component = nullptr;

        // 更新的排行榜操作
        std::unordered_map< uint64, std::set< KFRankSetting* > > _update_rank_list;
    };
}

#endif