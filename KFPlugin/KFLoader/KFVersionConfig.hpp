#ifndef __KF_VERSION_CONFIG_H__
#define __KF_VERSION_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFVersionSetting : public KFStrSetting
    {
    public:
        // 版本号
        std::string _version;
    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFVersionConfig : public KFConfigT< KFVersionSetting >, public KFInstance< KFVersionConfig >
    {
    public:
        KFVersionConfig()
        {
            _key_name = "name";
            _file_name = "version";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFVersionSetting* kfsetting )
        {
            kfsetting->_version = xmlnode.ReadString( "version" );
        }
    };
}

#endif