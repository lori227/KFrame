#include "KFClusterMasterModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterMasterModule::OnServerLostClusterProxy );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterRegisterToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterUpdateToMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ, &KFClusterMasterModule::HandleClusterAuthToMasterReq );
    }

    void KFClusterMasterModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_TO_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_TO_MASTER_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterMasterModule::AddProxyServer( const std::string& name, const std::string& type, uint64 id, const std::string& ip, uint32 port, uint32 clientcount )
    {
        auto kfgate = _kf_proxy_list.Find( id );
        if ( kfgate == nullptr )
        {
            kfgate = __KF_NEW__( KFProxyData );
            kfgate->_id = id;
            kfgate->_type = type;
            kfgate->_name = name;
            kfgate->_ip = ip;
            kfgate->_port = port;
            _kf_proxy_list.Insert( id, kfgate );
        }

        kfgate->_client_count = clientcount;
    }

    void KFClusterMasterModule::UpdateProxyServer( uint64 handleid, uint32 clientcount )
    {
        auto kfgate = _kf_proxy_list.Find( handleid );
        if ( kfgate == nullptr )
        {
            return;
        }

        kfgate->_client_count = clientcount;
    }

    void KFClusterMasterModule::RemoveProxyServer( uint64 handleid )
    {
        _kf_proxy_list.Remove( handleid );
    }

    KFProxyData* KFClusterMasterModule::SelectProxyServer()
    {
        auto maxclientcount = __MAX_UINT32__;
        KFProxyData* kfproxydata = nullptr;
        for ( auto& iter : _kf_proxy_list._objects )
        {
            auto temp = iter.second;
            if ( temp->_client_count < maxclientcount )
            {
                maxclientcount = temp->_client_count;
                kfproxydata = temp;
            }
        }

        // 先加一个连接数量, 避免同一时间都选择同有一个服务器
        if ( kfproxydata != nullptr )
        {
            ++kfproxydata->_client_count;
        }

        return kfproxydata;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFClusterMasterModule::OnServerLostClusterProxy )
    {
        __LOG_ERROR__( "[{}:{}:{}] lost!", handlename, handletype, KFAppID::ToString( handleid ) );

        if ( handlename == __KF_STRING__( proxy ) )
        {
            RemoveProxyServer( handleid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterRegisterToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterRegisterToMasterReq );

        // SendAllocShardToProxy( kfmsg.id() );
        __LOG_DEBUG__( "[{}:{}:{}|{}:{}] discovered!", kfmsg.name(), kfmsg.type(), KFAppID::ToString( kfmsg.id() ), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterUpdateToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterUpdateToMasterReq );

        AddProxyServer( kfmsg.name(), kfmsg.type(), kfmsg.id(), kfmsg.ip(), kfmsg.port(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterAuthToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthToMasterReq );

        auto strclientid = KFAppID::ToString( kfmsg.clientid() );
        __LOG_DEBUG__( "cluster client[{}:{}] auth req!", strclientid, kfmsg.clusterkey() );

        static auto _cluster_key = "cluster@kframe,./";
        if ( kfmsg.clusterkey() != _cluster_key )
        {
            return __LOG_ERROR__( "cluster client[{}] key[{}!={}] error!", strclientid, kfmsg.clusterkey(), _cluster_key );
        }

        auto kfproxy = SelectProxyServer();
        if ( kfproxy == nullptr )
        {
            return __LOG_ERROR__( "cluster client[{}] no proxy error!", strclientid );
        }

        __LOG_DEBUG__( "cluster client[{}] auth proxy[{}] ok!", strclientid, KFAppID::ToString( kfproxy->_id ) );

        // 创建token
        auto id = KFGlobal::Instance()->Make64Guid();
        std::string md5source = __FORMAT__( "{}:{}", id, kfmsg.clientid() );
        auto token = KFCrypto::Md5Encode( md5source );

        {
            // Token发送给ProxyServer
            KFMsg::S2SClusterTokenToProxyReq req;
            req.set_token( token );
            req.set_clientid( kfmsg.clientid() );
            _kf_tcp_server->SendNetMessage( kfproxy->_id, KFMsg::S2S_CLUSTER_TOKEN_TO_PROXY_REQ, &req );
        }

        KFMsg::S2SClusterAuthToClientAck ack;
        ack.set_token( token );
        ack.set_name( kfproxy->_name );
        ack.set_type( kfproxy->_type );
        ack.set_id( kfproxy->_id );
        ack.set_ip( kfproxy->_ip );
        ack.set_port( kfproxy->_port );
        _kf_tcp_server->SendNetMessage( kfmsg.clientid(), KFMsg::S2S_CLUSTER_AUTH_TO_CLIENT_ACK, &ack );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}