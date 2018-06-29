#ifndef __KF_CLUSTER_SERVER_CONFIG_H__
#define __KF_CLUSTER_SERVER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFClusterServerConfig : public KFConfig, public KFSingleton< KFClusterServerConfig >
    {
    public:
        KFClusterServerConfig();
        ~KFClusterServerConfig();

        bool LoadConfig( const char* file );

    public:
        // 集群的认证key
        std::string _cluster_key;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_cluster_server_config = KFClusterServerConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif