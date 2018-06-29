#include "KFDataShardConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFDataShardConfig::KFDataShardConfig()
    {

    }

    KFDataShardConfig::~KFDataShardConfig()
    {

    }

    bool KFDataShardConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto zonenode = config.FindNode( "Zone" );
            while ( zonenode.IsValid() )
            {
                auto zoneid = zonenode.GetUInt32( "Id" );

                auto servernode = zonenode.FindNode( "Server" );
                while ( servernode.IsValid() )
                {
                    auto serverid = servernode.GetUInt32( "Id" );
                    AddServerZone( zoneid, serverid );
                    servernode.NextNode();
                }
                zonenode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    const std::set< uint32 >* KFDataShardConfig::FindZoneId( uint32 serverid )
    {
        auto iter = _server_zone_list.find( serverid );
        if ( iter == _server_zone_list.end() )
        {
            return nullptr;
        }

        return &iter->second;
    }

    void KFDataShardConfig::AddServerZone( uint32 zoneid, uint32 serverid )
    {
        auto iter = _server_zone_list.find( serverid );
        if ( iter == _server_zone_list.end() )
        {
            std::set< uint32 > zonelist;
            iter = _server_zone_list.insert( std::make_pair( serverid, zonelist ) ).first;
        }

        iter->second.insert( zoneid );
    }
}