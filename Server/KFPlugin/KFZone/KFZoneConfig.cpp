#include "KFZoneConfig.h"

namespace KFrame
{
    bool KFZoneConfig::LoadConfig()
    {
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto defaultnode = config.FindNode( "Default" );
        _zone._logic_id = defaultnode.GetUInt32( "LogicId" );
        _zone._name = defaultnode.GetString( "Name" );
        /////////////////////////////////////////////////////////////////////////////////////
        auto zones = config.FindNode( "Zones" );
        auto xmlnode = zones.FindNode( "Zone" );
        while ( xmlnode.IsValid() )
        {
            auto channel = xmlnode.GetUInt32( "Channel" );
            auto service = xmlnode.GetUInt32( "Service" );
            if ( KFGlobal::Instance()->CheckChannelService( channel, service ) )
            {
                auto id = xmlnode.GetUInt32( "Id" );
                if ( KFGlobal::Instance()->_zone_id == id )
                {
                    _zone._id = id;
                    _zone._logic_id = xmlnode.GetUInt32( "LogicId" );
                    _zone._name = xmlnode.GetString( "Name" );
                    _zone._ip = xmlnode.GetString( "Ip", true );
                    break;
                }
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}