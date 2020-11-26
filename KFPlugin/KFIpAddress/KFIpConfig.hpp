#ifndef __KF_IP_ADDRESS_CONFIG_H__
#define __KF_IP_ADDRESS_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFIpSetting : public KFIntSetting
    {
    public:

    };
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFIpConfig : public KFConfigT<KFIpSetting>, public KFInstance< KFIpConfig >
    {
    public:
        KFIpConfig()
        {
            _file_name = "ip";
        }
        ~KFIpConfig() = default;

    public:
        virtual void ReadSetting( KFXmlNode& xmlnode, KFIpSetting* kfsetting )
        {
            auto name = xmlnode.ReadString( "name", false );
            auto key = xmlnode.ReadUInt32( "key", true );
            auto value = xmlnode.ReadString( "value", false );
            KFGlobal::Instance()->AddConstant( name, key, value );
        }
    };
}

#endif