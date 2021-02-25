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
        KFModuleFunction<KFResetFunction> _function;
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
        virtual uint64 CalcNextResetTime( EntityPtr player, uint32 time_id );

        // 设置重置时间
        virtual void SetResetTime( EntityPtr player, uint32 time_id, uint64 now_time );

    protected:
        // 添加重置函数
        virtual void AddResetFunction( const std::string& function_name, uint32 count, KFModule* module, KFResetFunction& function );

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& function_name );
    protected:
        // 进入游戏
        __KF_PLAYER_ENTER_FUNCTION__( OnEnterResetModule );

        // 重置数据
        __KF_PLAYER_RESET_FUNCTION__( ResetPlayerData );

        // 刷新数据
        __KF_PLAYER_RUN_FUNCTION__( RunResetPlayerData );

        // 判断所有重置时间
        UInt64Map& UpdateAllResetTime( EntityPtr player, DataPtr time_record );

        // 判断重置时间, 返回是否成功, 上次重置时间
        std::tuple<bool, uint64> UpdateResetTime( EntityPtr player, DataPtr time_record, std::shared_ptr<const KFTimeLoopSetting> setting );

        // 重置玩家属性
        void ResetPlayerData( EntityPtr player, const ResetData* reset_data );

        // 重置逻辑
        void ResetPlayerLogic( EntityPtr player, uint32 time_id, uint64 last_reset_time, const std::string& function_name );
    private:
        // 注册的重置逻辑
        KFHashMap<std::string, KFResetLogicData> _reset_logic_list;
    };
}



#endif