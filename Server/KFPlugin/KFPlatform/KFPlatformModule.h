#ifndef __KF_PLATFORM_MODULE_H__
#define __KF_PLATFORM_MODULE_H__

/************************************************************************
//    @Module			:    平台逻辑相关
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-9
************************************************************************/

#include "KFPlatformInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFOption/KFOptionInterface.h"

namespace KFrame
{
    class KFPlatformModule : public KFPlatformInterface
    {
    public:
        KFPlatformModule() = default;
        ~KFPlatformModule() = default;

        virtual void InitModule();

        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        // 获得api地址
        virtual const std::string& GetPlatformApiUrl();

        // 创建平台连接
        virtual const std::string& MakePlatformUrl( const std::string& path );

    protected:
        const std::string& MakePlatformSign( uint32 nowtime );
    };
}



#endif