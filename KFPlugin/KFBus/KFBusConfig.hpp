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
    class KFBusConfig : public KFConfigT<KFBusSetting>, public KFInstance<KFBusConfig>
    {
    public:
        KFBusConfig()
        {
            _key_name = "id";
            _file_name = "bus";
        }

    public:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFBusSetting> setting )
        {
            setting->_app_name = xml_node.ReadString( "appname", false );
            setting->_app_type = xml_node.ReadString( "apptype", false );
            setting->_app_id = xml_node.ReadString( "appid", false );
            setting->_connect_name = xml_node.ReadString( "connectname", false );
            setting->_connect_type = xml_node.ReadString( "connecttype", false );
            setting->_connect_id = xml_node.ReadString( "connectid", false );
            setting->_interval = xml_node.ReadUInt32( "interval", false );
            setting->_multi = xml_node.ReadUInt32( "multi", false );
        }
    };
}

#endif