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
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFIpAddress/KFIpAddressInterface.h"
#include "KFZConfig/KFRouteConfig.hpp"

namespace KFrame
{
    // proxy数据
    class KFProxyData
    {
    public:
        // id
        uint64 _id = 0u;

        // type
        std::string _type;

        // name
        std::string _name;

        // ip
        std::string _ip;

        // 端口
        uint32 _port = 0u;

        // 当前连接的客户端
        uint32 _client_count = 0u;

        // 所在的Master服务器id
        uint64 _master_id = 0u;
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
        // 注册proxy
        __KF_MESSAGE_FUNCTION__( HandleClusterRegisterToMasterReq );
        __KF_MESSAGE_FUNCTION__( HandleClusterSyncProxyToMasterReq );

        // 登录认证
        __KF_MESSAGE_FUNCTION__( HandleClusterAuthToMasterReq );

        // 丢失proxy
        __KF_MESSAGE_FUNCTION__( HandleClusterLostProxyToMasterReq );

        // token
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenToMasterReq );
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenToProxyAck );
        __KF_MESSAGE_FUNCTION__( HandleClusterTokenToMasterAck );

    protected:
        // 连接Mster
        __KF_NET_EVENT_FUNCTION__( OnClientConnectClusterMaster );

        // master 丢失
        __KF_NET_EVENT_FUNCTION__( OnClientLostClusterMaster );

        // 连接丢失
        __KF_NET_EVENT_FUNCTION__( OnServerLostClusterProxy );

        // 选择proxy
        KFProxyData* SelectProxyServer( uint64 clientid );

        // 创建Token
        std::string CreateToken( uint64 clientid );

        // 删除Token
        std::string FindToken( uint64 clientid );

    private:
        // proxy连接列表
        KFConHash _proxy_hash;
        KFHashMap< uint64, uint64, KFProxyData > _kf_proxy_list;

        // token列表
        std::unordered_map< uint64, std::string > _token_list;
    };
}



#endif