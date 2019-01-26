#ifndef __KF_FILTER_INTERFACE_H__
#define __KF_FILTER_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    // 屏蔽字符检查模块
    class KFFilterInterface : public KFModule
    {
    public:
        // 检查是否有屏蔽字符
        virtual bool CheckFilter( const std::string& source ) = 0;

        // 检查屏蔽字符并替换成 **
        virtual bool CensorFilter( std::string& source ) = 0;
    };

    ///////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_filter, KFFilterInterface );
    ///////////////////////////////////////////////////////////////////////

}



#endif