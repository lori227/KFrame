#ifndef __KF_PLATFORM_INTERFACE_H__
#define __KF_PLATFORM_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFPlatformInterface : public KFModule
    {
    public:
        // 获得api地址
        virtual const std::string& GetPlatformApiUrl() = 0;

        // 创建平台连接
        virtual const std::string& MakePlatformUrl( const std::string& path ) = 0;
    };

    /////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_platform, KFPlatformInterface );
    /////////////////////////////////////////////////////////////////////////
}

#endif