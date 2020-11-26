#ifndef __KF_BUS_CONFIG_H__
#define __KF_BUS_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFBusSetting : public KFIntSetting
    {
    public:
        std::string _app_name;
        std::string _app_type;
        std::string _app_id;

        std::string _connect_name;
        std::string _connect_type;
        std::string _connect_id;

        // 连接间隔
        uint32 _interval = _invalid_int;

        // 数量比
        uint32 _multi = _invalid_int;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFBusConfig : public KFConfigT< KFBusSetting >, public KFInstance< KFBusConfig >
    {
    public:
        KFBusConfig()
        {
            _key_name = "id";
            _file_name = "bus";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFBusSetting* kfsetting )
        {
            kfsetting->_app_name = xmlnode.ReadString( "appname", false );
            kfsetting->_app_type = xmlnode.ReadString( "apptype", false );
            kfsetting->_app_id = xmlnode.ReadString( "appid", false );
            kfsetting->_connect_name = xmlnode.ReadString( "connectname", false );
            kfsetting->_connect_type = xmlnode.ReadString( "connecttype", false );
            kfsetting->_connect_id = xmlnode.ReadString( "connectid", false );
            kfsetting->_interval = xmlnode.ReadUInt32( "interval", false );
            kfsetting->_multi = xmlnode.ReadUInt32( "multi", false );
        }
    };
}

#endif