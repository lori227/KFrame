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
#include "KFResetConfig.hpp"
#include "KFTimeConfig.hpp"

namespace KFrame
{
    class KFResetLogicData
    {
    public:
        std::string _module;

        // 次数
        uint32 _count = 0u;

        // 回调函数
        KFResetFunction _function = nullptr;
    };

    class KFResetLogicSetting
    {
    public:
        // 时间逻辑
        const KFTimeSetting* _time_setting = nullptr;

        // 重置逻辑
        KFVector< KFResetLogicData > _reset_logic_data;
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
        // 判断是否重置时间是否ok
        virtual bool CheckResetTime( KFEntity* player, uint32 timeid );

        // 获得下一次重置时间
        virtual uint64 CalcNextResetTime( uint64 time, uint32 timeid );

        // 重置时间
        virtual void ResetTime( KFEntity* player, uint32 timeid );
    protected:
        // 添加重置函数
        virtual void AddResetFunction( uint32 timeid, uint32 count, const std::string& module, KFResetFunction& function );

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& module );
    protected:
        // 进入游戏
        __KF_ENTER_PLAYER_FUNCTION__( OnEnterResetModule );

        // 重置数据
        __KF_RESET_PLAYER_FUNCTION__( ResetPlayerData );

        // 刷新数据
        __KF_RUN_PLAYER_FUNCTION__( RunResetPlayerData );

        // 判断所有
        bool UpdateAllResetTime( KFEntity* player, KFData* kftimerecord );
        bool UpdateResetTime( KFEntity* player, KFData* kftimerecord, const KFTimeSetting* kfsetting );

        // 判断是否重置时间是否ok
        bool CheckResetTimeData( KFData* kftimerecord, uint32 timeid );

        // 重置配置属性
        void ResetConfigData( KFEntity* player, KFData* kftimerecord );

        // 重置逻辑
        void ResetPlayerLogic( KFEntity* player, KFData* kftimerecord );
        void ResetPlayerLogicCount( KFEntity* player, const KFTimeData* timedata, const KFResetLogicData* resetdata, uint64 lasttime, uint64 nowtime );
    private:
        // 注册的重置逻辑
        KFHashMap< uint32, uint32, KFResetLogicSetting > _reset_data_list;
    };
}



#endif