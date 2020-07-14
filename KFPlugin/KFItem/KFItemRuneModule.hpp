#ifndef __KF_ITEM_RUNE_MOUDLE_H__
#define __KF_ITEM_RUNE_MOUDLE_H__

/************************************************************************
//    @Module			:    符石系统
//    @Author           :    erlking
//    @QQ				:    729159320
//    @Mail			    :    729159320@qq.com
//    @Date             :    2020-2-27
************************************************************************/

#include "KFrameEx.h"
#include "KFItemInterface.h"
#include "KFItemRuneInterface.h"
#include "KFItemMoveInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRealm/KFRealmData.hpp"
#include "KFRealm/KFRealmInterface.h"
#include "KFZConfig/KFItemConfig.hpp"
#include "KFZConfig/KFItemTypeConfig.hpp"

namespace KFrame
{
    class KFItemRuneModule : public KFItemRuneInterface
    {
    public:
        // 合成数据
        class KFCompoundData
        {
        public:
            uint32 runeid = 0u;			// 符石id
            uint32 compoundid = 0u;		// 合成id
            uint32 compoundnum = 0u;	// 合成符石数量
            UInt64Vector removelist;	// 删除物品列表(uuid)
            UInt32Map addlist;			// 添加符石列表(itemid itemnum)
            uint64 replaceuuid = 0u;	// 符石槽替换uuid
            uint32 replaceid = 0u;		// 符石槽替换itemid
        };

    public:
        KFItemRuneModule() = default;
        ~KFItemRuneModule() = default;

        // 逻辑
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterRuneModule );

        // 移动符石
        __KF_MESSAGE_FUNCTION__( HandleMoveRuneReq );


        // 添加符石回调
        __KF_ADD_DATA_FUNCTION__( OnAddRuneItemCallBack );

        // 添加符石延时处理
        __KF_TIMER_FUNCTION__( OnDelayTimerOperate );

    protected:
        // 符石槽中是否有相同类型的符石
        uint32 CheckSameRuneType( KFEntity* player, KFData* kfitem, KFData* kftargetrecord, uint32 excludeindex = 0u );

        // 装备符石
        void PutOnRune( KFEntity* player, const UInt64Set& uuidlist );

        // 延时装备合成符石
        void OnDelayPutOnAndCompoundRune( KFEntity* player, bool is_send = true );

        // 延时合成符石
        void OnDelayCompoundRune( KFEntity* player, const UInt32Set& runeset, bool is_send );

        // 合并符石
        void CompoundRune( KFEntity* player, KFCompoundData& compounddata );

        // 添加秘境符石数据
        bool AddRealmRuneData( KFEntity* player, uint32 runeid );

    private:
        // 玩家上下文组件
        KFComponent* _kf_component = nullptr;

        // 获得符石列表
        std::unordered_map< uint64, UInt64Set > _add_rune_data;
    };
}

#endif