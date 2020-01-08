#include "KFRouteConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFRouteConfig::LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        _route_list.clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( filepath );
        auto config = kfxml.RootNode();

        auto clusternode = config.FindNode( "Cluster" );
        _cluster_name = clusternode.GetString( "Name" );
        _cluster_key = clusternode.GetString( "Key" );
        //////////////////////////////////////////////////////////////////
        auto routesnode = config.FindNode( "Routes" );
        auto routenode = routesnode.FindNode( "Route" );
        while ( routenode.IsValid() )
        {
            auto source = routenode.GetString( "Source" );
            auto target = routenode.GetString( "Target" );
            _route_list[ source ] = target;

            routenode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}