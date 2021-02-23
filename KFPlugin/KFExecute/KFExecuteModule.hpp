#ifndef __KF_EXECUTE_MOUDLE_H__
#define __KF_EXECUTE_MOUDLE_H__

/************************************************************************
//    @Module			:    执行逻辑回调系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-9-24
************************************************************************/

#include "KFExecuteInterface.h"
#include "KFDrop/KFDropInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDeployClient/KFDeployClientInterface.h"
#include "KFConfig/KFExecuteConfig.hpp"

namespace KFrame
{
    class KFExecuteModule : public KFExecuteInterface
    {
    public:
        KFExecuteModule() = default;
        ~KFExecuteModule() = default;

        virtual void BeforeRun();
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////

        virtual bool Execute( EntityPtr player, uint32 executeid, const char* function, uint32 line );
        virtual bool Execute( EntityPtr player, uint32 executeid, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

        virtual bool Execute( EntityPtr player, const UInt32Vector& executelist, const char* function, uint32 line );
        virtual bool Execute( EntityPtr player, const UInt32Vector& executelist, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

        virtual bool Execute( EntityPtr player, const ExecuteDataPtr& executedata, const char* function, uint32 line );
        virtual bool Execute( EntityPtr player, const ExecuteDataPtr& executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

    protected:
        virtual void BindExecuteFunction( KFModule* module, const std::string& name, KFExecuteFunction& function );
        virtual void UnBindExecuteFunction( const std::string& name );

        // 设置执行开关
        void SetExecuteOpen( const std::string& name, bool isopen );
    protected:

        // 关闭, 开启执行功能
        __KF_DEPLOY_FUNCTION__( OnDeployExecuteClose );
        __KF_DEPLOY_FUNCTION__( OnDeployExecuteOpen );

        // 执行添加道具
        __KF_EXECUTE_FUNCTION__( OnExecuteAddData );

        // 执行掉落逻辑
        __KF_EXECUTE_FUNCTION__( OnExecuteDropLogic );

        // 执行逻辑
        __KF_EXECUTE_FUNCTION__( OnExecuteQueueLogic );

    protected:
        // 执行列表
        KFMapFunction< std::string, KFExecuteFunction > _execute_function;
    };
}

#endif