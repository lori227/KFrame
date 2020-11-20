#ifndef __KF_PROJECT_CONFIG_H__
#define __KF_PROJECT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFProjectSetting : public KFStrSetting
    {
    public:
        // name
        std::string _name;

        // value
        std::string _value;
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFProjectConfig : public KFConfigT< KFProjectSetting >, public KFInstance< KFProjectConfig >
    {
    public:
        KFProjectConfig()
        {
            _key_name = "name";
            _file_name = "project";
        }

        ~KFProjectConfig() = default;

    protected:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFProjectSetting* kfsetting )
        {
            kfsetting->_name = xmlnode.ReadString( "name", true );
            kfsetting->_value = xmlnode.ReadString( "value", true );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif