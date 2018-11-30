#ifndef __KF_CLUSTER_MASTER_MANAGE_H__
#define __KF_CLUSTER_MASTER_MANAGE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFProxyData
    {
    public:
        KFProxyData()
        {
            _client_count = 0;
        }

        // id
        uint64 _id;

        // type
        std::string _type;

        // name
        std::string _name;

        // ip
        std::string _ip;

        // 端口
        uint32 _port;

        // 当前连接的客户端
        uint32 _client_count;
    };

    // 集群管理器
    class KFProxyManage : public KFSingleton< KFProxyManage >
    {
    public:
        KFProxyManage() = default;
        ~KFProxyManage() = default;

        // 添加ProxyServer
        void AddProxyServer( const std::string& type, uint64 handleid, const std::string& name, const std::string& ip, uint32 port, uint32 clientcount );

        // 删除ProxyServer
        void RemoveProxyServer( uint64 handleid );

        // 更新ProxyServer
        void UpdateProxyServer( uint64 handleid, uint32 clientcount );

        // 选择ProxyServer
        KFProxyData* SelectProxyServer();
        KFProxyData* HashProxyServer( uint64 clientid );

        // 查找代理服务器
        KFProxyData* FindProxyServer( uint64 handleid );

    public:
        // gate连接列表
        KFMap< uint64, uint64, KFProxyData > _kf_proxy_list;

        // hash一致性列表
        KFConHash _kf_hash;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_proxy_manage = KFProxyManage::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif