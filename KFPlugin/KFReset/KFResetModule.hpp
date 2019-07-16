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
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"

namespace KFrame
{
    class KFEntity;
    class KFResetModule : public KFResetInterface
    {
    public:
        KFResetModule() = default;
        ~KFResetModule() = default;

        // 加载配置
        virtual void InitModule();

        virtual void BeforeRun();
        virtual void AfterRun();

        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 重置数据
        __KF_RESET_PLAYER_FUNCTION__( ResetPlayerData );

        // 刷新数据
        __KF_RUN_PLAYER_FUNCTION__( RunResetPlayerData );

        // 判断刷新时间
        bool CheckResetPlayerDataTime();

        // 计算刷新时间
        void CalcNextResetTime();

    private:
        // 是否需要刷新
        bool _need_to_reset = false;

        // 上次刷新时间
        uint64 _next_reset_data_time = 0;
    };
}



#endif