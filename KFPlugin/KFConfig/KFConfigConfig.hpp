#ifndef __KF_CONFIG_CONFIG_H__
#define __KF_CONFIG_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFConfigSetting : public KFStrSetting
    {
    public:
        // 是否是配置文件
        const std::string& IsFile( const std::string& file ) const;

    public:
        // 加载标记
        uint32 _load_mask = 0u;

        // 配置文件列表
        std::vector< std::string > _file_list;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFConfigConfig : public KFStrConfigT< KFConfigSetting >
    {
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFConfigSetting* kfsetting );
    };
}

#endif