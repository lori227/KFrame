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

        // 倍数属性
        bool IsMultipleData( const std::string& dataname ) const;

    public:

        // 属性配置的文件名
        std::string _class_file;

        // 全局类名
        std::string _global_class_name;

        // 延迟保持时间
        uint32 _delay_save_time;

        // 允许有倍数的属性
        std::set< std::string > _multiple_data;
    };

    ///////////////////////////////////////////////////////////////////////////
    static auto _kf_kernel_config = KFKernelConfig::Instance();
    ///////////////////////////////////////////////////////////////////////////
}
#endif