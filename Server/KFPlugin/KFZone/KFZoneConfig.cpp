#include "KFZoneConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFZoneConfig::KFZoneConfig()
    {

    }

    KFZoneConfig::~KFZoneConfig()
    {

    }

    bool KFZoneConfig::LoadConfig()
    {
        _zone_list.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto zones = config.FindNode( "Zones" );
        auto xmlnode = zones.FindNode( "Zone" );
        while ( xmlnode.IsValid() )
        {
            auto kfzone = __KF_CREATE__( KFZone );

            kfzone->_channel = xmlnode.GetUInt32( "Channel" );
            kfzone->_id = xmlnode.GetUInt32( "Id" );
            kfzone->_name = xmlnode.GetString( "Name" );
            kfzone->_logic_id = xmlnode.GetUInt32( "LogicId" );
            kfzone->_ip = xmlnode.GetString( "Ip", true );

            ZoneKey zonekey( kfzone->_channel, kfzone->_id );
            _zone_list.Insert( zonekey, kfzone );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFZone* KFZoneConfig::FindZone( uint32 appflag, uint32 zoneid )
    {
        ZoneKey zonekey( appflag, zoneid );
        return _zone_list.Find( zonekey );
    }
}