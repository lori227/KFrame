#ifndef __KF_PROJECT_MOUDLE_H__
#define __KF_PROJECT_MOUDLE_H__

/************************************************************************
//    @Module			:    项目配置
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-7-4
************************************************************************/


#include "KFProjectInterface.h"
#include "KFProjectConfig.hpp"

namespace KFrame
{
    class KFProjectModule : public KFProjectInterface
    {
    public:
        KFProjectModule() = default;
        ~KFProjectModule() = default;

        // 初始化
        virtual void BeforeRun();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    };
}



#endif