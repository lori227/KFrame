#ifndef __KF_KERNEL_CONFIG_H__
#define __KF_KERNEL_CONFIG_H__

#include "KFConfig.h"
#include "KFSingleton.h"
#include "KFCore/KFDataSetting.h"

namespace KFrame
{
    class KFKernelConfig : public KFStrConfigT< KFClassSetting >, public KFSingleton< KFKernelConfig >
    {
    public:
        KFKernelConfig( const std::string& file, bool isclear )
            : KFStrConfigT< KFClassSetting >( file, isclear )
        {
        }

        // 加载完成
        virtual void LoadComplete();

        // 获得属性配置
        const KFDataSetting* FindDataSetting( const std::string& classname, const std::string& dataname );
    protected:
        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFClassSetting* kfsetting );
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_kernel_config = KFKernelConfig::Instance( "class.xml", false );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif