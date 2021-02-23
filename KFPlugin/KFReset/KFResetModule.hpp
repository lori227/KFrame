#ifndef __KF_RESET_MODULE_H__
#define __KF_RESET_MODULE_H__

/************************************************************************
//    @Module			:    数据重置模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-10
************************************************************************/

#include "KFResetInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFConfig/KFResetConfig.hpp"
#include "KFConfig/KFTimeLoopConfig.hpp"

namespace KFrame
{
    class KFResetLogicData
    {
    public:
        // 次数
        uint32 _count = 0u;

        // 回调函数
        KFResetFunction _function = nullptr;
    };

    class KFResetModule : public KFResetInterface
    {
    public:
        KFResetModule() = default;
        ~KFResetModule() = default;

        virtual void BeforeRun();

        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 获得下一次重置时间
        virtual uint64 CalcNextResetTime( EntityPtr player, uint32 timeid );

        // 设置重置时间
        virtual void SetResetTime( EntityPtr player, uint32 timeid, uint64 now_time );

    protected:
        // 添加重置函数
        virtual void AddResetFunction( const std::string& functionname, uint32 count, KFResetFunction& function );

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& functionname );
    protected:
        // 进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterResetModule );

        // 重置数据
        __KF_PLAYER_RESET_FUNCTION__( ResetPlayerData );

        // 刷新数据
        __KF_PLAYER_RUN_FUNCTION__( RunResetPlayerData );

        // 判断所有重置时间
        UInt64Map& UpdateAllResetTime( EntityPtr player, DataPtr kftimerecord );

        // 判断重置时间, 返回是否成功, 上次重置时间
        std::tuple<bool, uint64> UpdateResetTime( EntityPtr player, DataPtr kftimerecord, const KFTimeLoopSetting* setting );

        // 重置玩家属性
        void ResetPlayerData( EntityPtr player, const ResetData* resetdata );

        // 重置逻辑
        void ResetPlayerLogic( EntityPtr player, uint32 timeid, uint64 lastresettime, const std::string& functionname );
    private:
        // 注册的重置逻辑
        KFHashMap< std::string, KFResetLogicData > _reset_logic_list;
    };
}



#endif