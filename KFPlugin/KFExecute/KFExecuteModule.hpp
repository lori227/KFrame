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

namespace KFrame
{
    class KFExecuteModule : public KFExecuteInterface
    {
    public:
        KFExecuteModule() = default;
        ~KFExecuteModule() = default;
        ////////////////////////////////////////////////////////////////////////////////
        virtual bool Execute( KFEntity* player, const KFExecuteData* executedata, const char* function, uint32 line );
        virtual bool Execute( KFEntity* player, const KFExecuteData* executedata, const std::string& modulename, uint64 moduleid, const char* function, uint32 line );

    protected:
        virtual void BindExecuteFunction( const std::string& name, KFExecuteFunction& function );
        virtual void UnBindExecuteFunction( const std::string& name );

    protected:
        // 执行列表
        KFBind < std::string, const std::string&, KFExecuteFunction > _execute_function;

    };
}

#endif