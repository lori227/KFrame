#ifndef __KF_VERSION_CONFIG_H__
#define __KF_VERSION_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFVersionSetting : public KFStrSetting
    {
    public:
        bool IsNeedReload() const
        {
            return _old_version != _new_version;
        }

    public:
        // 老版本号
        std::string _old_version;

        // 新版本号
        std::string _new_version;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFVersionConfig : public KFConfigT< KFVersionSetting >
    {
    public:
        KFVersionConfig()
        {
            _key_name = "name";
        }

        // 判断是否需要重新加载
        const std::string& CheckNeedReload( const std::string& filename );

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFVersionSetting* kfsetting );
    };
}

#endif