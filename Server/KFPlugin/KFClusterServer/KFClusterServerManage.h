#ifndef __KF_GATE_MANAGE_H__
#define __KF_GATE_MANAGE_H__

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
        uint32 _id;

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
        KFProxyManage();
        ~KFProxyManage();

        // 添加GateServer
        void AddProxyServer( const std::string& type, uint32 handleid, const std::string& name, const std::string& ip, uint32 port );

        // 删除GateServer
        void RemoveProxyServer( uint32 handleid );

        // 更新GateServer
        void UpdateProxyServer( uint32 handleid, uint32 clientcount );

        // 选择GateServer
        KFProxyData* SelectProxyServer();
        KFProxyData* HashProxyServer( uint32 clientid );

        // 查找代理服务器
        KFProxyData* FindProxyServer( uint32 handleid );

    public:
        // gate连接列表
        KFMap< uint32, uint32, KFProxyData > _kf_proxy_list;

        // hash一致性列表
        KFConHash _kf_hash;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_proxy_manage = KFProxyManage::Instance();
    ///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif