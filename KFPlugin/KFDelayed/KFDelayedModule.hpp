#ifndef __KF_DELAYED_MODULE_H__
#define __KF_DELAYED_MODULE_H__


/************************************************************************
//    @Module			:    定时功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-3-13
************************************************************************/

#include "KFDelayedData.hpp"
#include "KFDelayedInterface.h"

namespace KFrame
{
    class KFDelayedModule : public KFDelayedInterface
    {
    public:
        KFDelayedModule() = default;
        virtual ~KFDelayedModule();

        virtual void Run();
        virtual void BeforeRun();

    protected:
        virtual void AddDelayedFunction( uint64 time, uint64 objectid, const int8* data, uint32 size, const std::string& module, KFDelayedFunction& function );
        virtual void AddDelayedFunction( KFTimeData* timedata, uint64 objectid, const int8* data, uint32 size, const std::string& module, KFDelayedFunction& function );
        virtual void RemoveDelayedFunction( const std::string& module );
        virtual void RemoveDelayedFunction( const std::string& module, uint64 objectid );

    protected:
        // 添加定时任务
        void AddDelayedData( KFDelayedData* kfdata );

        // 删除定时任务
        void RemoveDelayedData( const std::string& module, uint64 objectid );

        void RunDelayedUpdate();
        void RunDelayedRegister();
        void RunDelayedRemove();

        // 计算下一次的执行时间
        void CalcNextCheckRunTime();

    private:
        // 下一次执行时间
        uint64 _next_check_run_time = 0u;

        // 需要注册的定时任务
        std::list< KFDelayedData* > _kf_delayed_register;

        // 计划任务
        std::unordered_map< std::string, std::unordered_map< uint64, KFDelayedData* > > _kf_delayed_data;

        // 需要删除的任务
        std::list< std::tuple< std::string, uint64 > > _kf_delayed_remove;

    };
}



#endif