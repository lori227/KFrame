#ifndef __KF_CLUSTER_MASTER_MODULE_H__
#define __KF_CLUSTER_MASTER_MODULE_H__

/************************************************************************
//    @Module			:    集群服务器模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-17
************************************************************************/

#include "KFClusterMasterInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

namespace KFrame
{
    // proxy数据
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

    class KFClusterMasterModule : public KFClusterMasterInterface
    {
    public:
        KFClusterMasterModule() = default;
        ~KFClusterMasterModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 注册
        __KF_MESSAGE_FUNCTION__( HandleClusterRegisterToMasterReq );

        // 更新连接数量
        __KF_MESSAGE_FUNCTION__( HandleClusterUpdateToMasterReq );

        // 登录认证
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthToMasterReq );

    protected:
        // 连接丢失
        __KF_SERVER_LOST_FUNCTION__( OnServerLostClusterProxy );

    protected:
        // 添加ProxyServer
        void AddProxyServer( const std::string& name, const std::string& type, uint64 id, const std::string& ip, uint32 port, uint32 clientcount );

        // 删除ProxyServer
        void RemoveProxyServer( uint64 handleid );

        // 更新ProxyServer
        void UpdateProxyServer( uint64 handleid, uint32 clientcount );

        // 选择ProxyServer
        KFProxyData* SelectProxyServer();
        KFProxyData* HashProxyServer( uint64 clientid );

        // 查找代理服务器
        KFProxyData* FindProxyServer( uint64 handleid );

    private:
        // gate连接列表
        KFMap< uint64, uint64, KFProxyData > _kf_proxy_list;

        // hash一致性列表
        KFConHash _kf_hash;
    };
}



#endif