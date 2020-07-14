#ifndef __KF_ITEM_WEAPON_MOUDLE_H__
#define __KF_ITEM_WEAPON_MOUDLE_H__

/************************************************************************
//    @Module			:    武器系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-10-16
************************************************************************/

#include "KFrameEx.h"
#include "KFItemInterface.h"
#include "KFItemWeaponInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFWeightConfig.hpp"
#include "KFZConfig/KFWeaponAffixConfig.hpp"
#include "KFZConfig/KFWeaponTypeConfig.hpp"
#include "KFZConfig/KFItemTypeConfig.hpp"

namespace KFrame
{
    class KFItemWeaponModule : public KFItemWeaponInterface
    {
    public:
        KFItemWeaponModule() = default;
        ~KFItemWeaponModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 初始化装备数据
        __KF_INIT_ITEM_FUNCTION__( InitWeaponData );

        // 武器耐久更新
        __KF_UPDATE_DATA_FUNCTION__( OnWeaponDurabilityUpdate );

        // 武器类型更新
        __KF_UPDATE_DATA_FUNCTION__( OnWeaponTypeUpdate );

        // 删除英雄
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveHero );

    protected:
        // 给英雄装备武器
        __KF_MESSAGE_FUNCTION__( HandleHeroWeaponReq );

        // 给英雄脱武器
        __KF_MESSAGE_FUNCTION__( HandleHeroUnWeaponReq );

        // 装备武器( 从一个英雄到另外一个英雄 )
        __KF_MESSAGE_FUNCTION__( HandleHeroWeaponAnotherReq );

        // 更新武器耐久
        __KF_MESSAGE_FUNCTION__( HandleUpdateDurabilityReq );

    protected:
        // 获得武器探索
        KFData* FindWeaponRecord( KFEntity* player );

        // 判断武器是否能状态
        uint32 CheckHeroWeapon( KFEntity* player, KFData* kfhero, KFData* kfweapon );

    private:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;
    };
}

#endif