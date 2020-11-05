#ifndef __KF_ROUTE_CONFIG_H__
#define __KF_ROUTE_CONFIG_H__

#include "KFrame.h"
#include "KFZConfig/KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRouteConfig : public KFConfig, public KFInstance< KFRouteConfig >
    {
    public:
        KFRouteConfig()
        {
            _file_name = "route";
        }

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask )
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

    public:
        // 转发集群名字
        std::string _cluster_name;
        // 转发集群认证
        std::string _cluster_key;

        // 转发关系
        StringMap _route_list;
    };
}

#endif