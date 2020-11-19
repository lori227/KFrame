#ifndef __KF_LOADER_CONFIG_H__
#define __KF_LOADER_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFConfigData
    {
    public:
        // 文件路径
        std::string _file_path;

        // 文件名
        std::string _file_name;

        // 加载标记
        uint32 _load_mask = 0u;
    };

    class KFLoaderSetting : public KFStrSetting
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
    class KFLoaderConfig : public KFConfigT< KFLoaderSetting >
    {
    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFLoaderSetting* kfsetting );
    };
}

#endif