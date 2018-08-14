#ifndef __KF_CLUSTER_SERVER_CONFIG_H__
#define __KF_CLUSTER_SERVER_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    class KFClusterSetting
    {
    public:
        KFClusterSetting()
        {
            _id = 0;
        }

    public:
        // 集群名字
        std::string _name;

        // 集群类型
        std::string _type;

        // 集群的秘钥
        std::string _key;

        // 集群服务器id
        uint32 _id;
    };

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFClusterServerConfig : public KFConfig, public KFSingleton< KFClusterServerConfig >
    {
    public:

        bool LoadConfig();

        const KFClusterSetting* FindClusterSetting( const std::string& name );

    public:
        // 集群客服端配置列表
        std::map< std::string, KFClusterSetting > _cluster_setting_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_cluster_config = KFClusterServerConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif