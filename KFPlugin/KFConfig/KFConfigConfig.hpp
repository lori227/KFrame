#ifndef __KF_CONFIG_CONFIG_H__
#define __KF_CONFIG_CONFIG_H__

#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    class KFConfigData
    {
    public:
        // 配置文件名
        std::string _file_name;

        // 加载标记
        uint32 _load_mask = 0u;
    };

    class KFConfigSetting : public KFStrSetting
    {
    public:
        // 是否是配置文件
        const KFConfigData* IsFile( const std::string& file ) const;

    public:
        // 配置文件列表
        std::vector< KFConfigData > _config_data_list;
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