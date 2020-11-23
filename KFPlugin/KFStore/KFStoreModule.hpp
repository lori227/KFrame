#ifndef __KF_STORE_MODULE_H__
#define __KF_STORE_MODULE_H__

/************************************************************************
//    @Module			:    商城系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-27
************************************************************************/

#include "KFStoreInterface.h"
#include "KFGoodsInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFStoreConfig.hpp"
#include "KFConfig/KFGoodsConfig.hpp"

namespace KFrame
{
    class KFStoreModule : public KFStoreInterface
    {
    public:
        KFStoreModule() = default;
        ~KFStoreModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        // 刷新商店
        virtual void RefreshGoods( KFEntity* player, uint32 storeid );

        // 删除随机商店
        virtual void RemoveStore( KFEntity* player, uint32 storeid );
    protected:
        // 请求购买商城道具
        __KF_MESSAGE_FUNCTION__( HandleStoreBuyGoodsReq );

        // 请求刷新商店
        __KF_MESSAGE_FUNCTION__( HandleStoreRefreshReq );

        // 进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterStoreModule );

        // 离开游戏
        __KF_PLAYER_LEAVE_FUNCTION__( OnLeaveStoreModule );

    protected:
        // 刷新商城
        __KF_TIMER_FUNCTION__( OnTimerRefreshStore );
        void StartRefreshStoreTimer( KFEntity* player, uint32 storeid, uint64 refreshtime );

        // 更新商店刷新时间
        void UpdateStoreRefreshTime( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord );

        // 刷新商城道具
        void StoreRefreshGoods( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord );

        // 获得刷新价格
        std::tuple< uint32, uint32 > StoreRefreshCostElemnts( KFEntity* player, const KFStoreSetting* kfsetting, uint32 refreshcount );

        // 重置刷新商店配置
        __KF_RESET_FUNCTION__( OnResetRefreshStoreCount );

    protected:
        // 玩家组件上下文
        KFComponent* _kf_component = nullptr;
    };

}

#endif