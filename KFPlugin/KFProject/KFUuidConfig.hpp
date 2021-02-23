#ifndef __KF_UUID_CONFIG_H__
#define __KF_UUID_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFUuidSetting : public KFStrSetting
    {
    public:
        // time
        uint32 _time = 0u;

        // zone
        uint32 _zone = 0u;

        // worker
        uint32 _worker = 0u;

        // seq
        uint32 _seq = 0u;
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFUuidConfig : public KFConfigT<KFUuidSetting>, public KFInstance<KFUuidConfig>
    {
    public:
        KFUuidConfig()
        {
            _key_name = "name";
            _file_name = "uuid";
        }

        ~KFUuidConfig() = default;

    protected:
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFUuidSetting> setting )
        {
            setting->_time = xml_node.ReadUInt32( "time", true );
            setting->_zone = xml_node.ReadUInt32( "zone", true );
            setting->_worker = xml_node.ReadUInt32( "worker", true );
            setting->_seq = xml_node.ReadUInt32( "seq", true );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif