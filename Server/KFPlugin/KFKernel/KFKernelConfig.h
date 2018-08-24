#ifndef __KF_KERNEL_CONFIG_H__
#define __KF_KERNEL_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFKernelConfig : public KFConfig, public KFSingleton< KFKernelConfig >
    {
    public:
        KFKernelConfig();
        ~KFKernelConfig();

        // 加载配置
        bool LoadConfig();

    public:

    };

    ///////////////////////////////////////////////////////////////////////////
    static auto _kf_kernel_config = KFKernelConfig::Instance();
    ///////////////////////////////////////////////////////////////////////////
}
#endif