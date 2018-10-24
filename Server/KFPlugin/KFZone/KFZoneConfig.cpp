#include "KFZoneConfig.h"

namespace KFrame
{
    KFZoneConfig::KFZoneConfig()
    {
        _default_zone = nullptr;
    }

    KFZoneConfig::~KFZoneConfig()
    {
        __KF_DESTROY__( KFZone, _default_zone );
    }

    bool KFZoneConfig::LoadConfig()
    {
        _zone_list.Clear();
        _default_zone = __KF_CREATE__( KFZone );
        //////////////////////////////////////////////////////////////////

        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto defaultnode = config.FindNode( "Default" );
        _default_zone->_logic_id = defaultnode.GetUInt32( "LogicId" );
        _default_zone->_name = defaultnode.GetString( "Name" );
        /////////////////////////////////////////////////////////////////////////////////////

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

    KFZone* KFZoneConfig::FindZone( uint32 appflag, uint32 zoneid )
    {
        ZoneKey zonekey( appflag, zoneid );
        auto kfzone = _zone_list.Find( zonekey );
        if ( kfzone == nullptr )
        {
            kfzone = _default_zone;
        }

        return kfzone;
    }
}