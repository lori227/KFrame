#ifndef __KF_PROJECT_MOUDLE_H__
#define __KF_PROJECT_MOUDLE_H__

/************************************************************************
//    @Module			:    项目配置模块
//    @Author           :    zux
//    @QQ				:    415906519
//    @Mail			    :    415906519@qq.com
//    @Date             :    2019-12-24
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
        // uint64配置
        virtual uint64 GetUInt64( const std::string& name )override;

        // double配置
        virtual double GetDouble( const std::string& name );

        // string配置
        virtual const std::string& GetString( const std::string& name );
    };
}



#endif