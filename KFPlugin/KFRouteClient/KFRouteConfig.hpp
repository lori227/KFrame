#ifndef __KF_ROUTE_CONFIG_H__
#define __KF_ROUTE_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRouteSetting : public KFStrSetting
    {
    public:
        // 源
        std::string _source;

        // 目标
        std::string _target;
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFRouteConfig : public KFConfigT< KFRouteSetting >, public KFInstance< KFRouteConfig >
    {
    public:
        KFRouteConfig()
        {
            _key_name = "source";
            _file_name = "route";
        }

        ~KFRouteConfig() = default;

    protected:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFRouteSetting* kfsetting )
        {
            kfsetting->_source = xmlnode.ReadString( "source", true );
            kfsetting->_target = xmlnode.ReadUInt32( "target", true );
        }

    };
}

#endif