#ifndef __KF_RESET_MODULE_H__
#define __KF_RESET_MODULE_H__

/************************************************************************
//    @Moudle			:    数据重置模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-10
************************************************************************/

#include "KFrame.h"
#include "KFResetInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFEntity;
    class KFResetModule : public KFResetInterface
    {
    public:
        KFResetModule();
        ~KFResetModule();

        // 加载配置
        virtual void InitModule();

        virtual void BeforeRun();

        virtual void ShutDown();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        void Run();

        // 重置数据
        void ResetPlayerData( KFEntity* player );

        // 刷新数据
        void RunResetPlayerData( KFEntity* player );

        // 判断刷新时间
        bool CheckResetPlayerDataTime();

        // 计算刷新时间
        void CalcNextResetTime();

    private:
        // 是否需要刷新
        bool _need_to_reset;

        // 上次刷新时间
        uint64 _next_reset_data_time;
    };
}



#endif