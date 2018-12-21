#ifndef __KF_CLUSTER_MASTER_CONFIG_H__
#define __KF_CLUSTER_MASTER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFClusterSetting
    {
    public:
        // 名字
        std::string _name;

        // key
        std::string _key;
    };

    /////////////////////////////////////////////////////////////////////////////////
    class KFClusterMasterConfig : public KFConfig, public KFSingleton< KFClusterMasterConfig >
    {
    public:
        KFClusterMasterConfig() = default;
        ~KFClusterMasterConfig() = default;

        // 加载配置
        bool LoadConfig();

        // 获得设置
        const KFClusterSetting* FindClusterSetting( const std::string& name );

    public:
        // 集群列表
        KFMap < std::string, const std::string&, KFClusterSetting > _cluster_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_cluster_master_config = KFClusterMasterConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif