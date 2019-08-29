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
#include "KFResetConfig.hpp"

namespace KFrame
{
    class KFResetData
    {
    public:
        // 时间数据
        KFTimeData _time_data;

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
        virtual void AfterRun();

        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加重置函数
        virtual void AddResetFunction( const KFTimeData& timedata, uint32 count, const std::string& module, KFResetFunction& function );

        // 删除重置函数
        virtual void RemoveResetFunction( const std::string& module );
    protected:
        // 重置数据
        __KF_RESET_PLAYER_FUNCTION__( ResetPlayerData );

        // 刷新数据
        __KF_RUN_PLAYER_FUNCTION__( RunResetPlayerData );

        // 判断刷新时间
        bool CheckResetPlayerDataTime();

        // 计算刷新时间
        void CalcNextResetTime();

        // 重置配置属性
        void ResetConfigData( KFEntity* player, KFDate& lastdate, KFDate& nowdate );

        // 重置逻辑
        void ResetPlayerLogic( KFEntity* player, KFDate& lastdate, KFDate& nowdate );
        void ResetPlayerLogicCount( KFEntity* player, KFResetData* kfresetdata, KFDate& lastdate, KFDate& nowdate );
    private:
        // 是否需要刷新
        bool _need_to_reset = false;

        // 上次刷新时间
        uint64 _next_reset_data_time = 0;

        // 注册的重置逻辑
        KFHashMap< std::string, const std::string&, KFResetData > _reset_data_list;
    };
}



#endif