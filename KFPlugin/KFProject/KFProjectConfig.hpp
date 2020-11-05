#ifndef __KF_PROJECT_CONFIG_H__
#define __KF_PROJECT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFProjectConfig : public KFConfig, public KFInstance< KFProjectConfig >
    {
    public:
        KFProjectConfig()
        {
            _file_name = "project";
        }

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

    public:
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFConstantConfig : public KFConfigT< KFIntSetting >, public KFInstance< KFConstantConfig >
    {
    public:
        KFConstantConfig()
        {
            _file_name = "constant";
        }

    protected:
        // 创建配置
        KFIntSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFIntSetting* kfsetting );
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif