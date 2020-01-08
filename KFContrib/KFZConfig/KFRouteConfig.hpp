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

        bool LoadConfig( const std::string& filename, const std::string& filepath, uint32 loadmask );

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