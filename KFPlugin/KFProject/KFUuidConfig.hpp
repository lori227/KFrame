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
    class KFUuidConfig : public KFConfigT< KFUuidSetting >, public KFInstance< KFUuidConfig >
    {
    public:
        KFUuidConfig()
        {
            _key_name = "name";
            _file_name = "uuid";
        }

        ~KFUuidConfig() = default;

    protected:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFUuidSetting* kfsetting )
        {
            kfsetting->_time = xmlnode.ReadUInt32( "time", true );
            kfsetting->_zone = xmlnode.ReadUInt32( "zone", true );
            kfsetting->_worker = xmlnode.ReadUInt32( "worker", true );
            kfsetting->_seq = xmlnode.ReadUInt32( "seq", true );
        }
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif