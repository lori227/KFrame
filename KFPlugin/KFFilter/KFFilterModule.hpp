#ifndef __KF_FILTER_MODULE_H__
#define __KF_FILTER_MODULE_H__


/************************************************************************
//    @Module			:    屏蔽字过滤模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-10-31
************************************************************************/

#include "KFFilterInterface.h"
#include "KFFilterConfig.hpp"

namespace KFrame
{
    //https://blog.csdn.net/u012755940/article/details/51689401/
    ////////////////////////////////////////////////////////////////
    class KFFilterModule : public KFFilterInterface
    {
    public:
        KFFilterModule() = default;
        ~KFFilterModule() = default;

        /////////////////////////////////////////////////////////////
        // 检查是否有屏蔽字符
        virtual bool CheckFilter( const std::string& source );

        // 检查屏蔽字符并替换成 **
        virtual bool CensorFilter( std::string& source );
    };
}



#endif