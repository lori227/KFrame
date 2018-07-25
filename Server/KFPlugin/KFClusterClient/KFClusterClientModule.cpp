#include "KFClusterClientModule.h"
#include "KFClusterClientConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFClusterClientModule::KFClusterClientModule()
    {
    }

    KFClusterClientModule::~KFClusterClientModule()
    {
        _kf_cluster_client.Clear();
    }

    void KFClusterClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_cluster_config, false );
    }

    void KFClusterClientModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFClusterClientModule::OnClientLostServer );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFClusterClientModule::OnClientConnectionServer );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_ACK, &KFClusterClientModule::HandleClusterAuthAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_ACK, &KFClusterClientModule::HandleClusterVerifyAck );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFClusterClientModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();

        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_VERIFY_ACK );

    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFClusterClientModule::AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function )
    {
        auto kfbind = _kf_connection_function.Create( name );
        kfbind->_function = function;
    }

    void KFClusterClientModule::RemoveConnectionFunction( const std::string& name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFClusterClientModule::CallClusterConnectionFunction( const std::string& name, uint32 serverid )
    {
        auto kfbind = _kf_connection_function.Find( name );
        if ( kfbind == nullptr )
        {
            return;
        }

        kfbind->_function( serverid );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFClusterClientModule::OnClientConnectionServer )
    {
        // 只处理除自己外的集群连接
        auto kfglobal = KFGlobal::Instance();
        if ( servername == kfglobal->_app_name )
        {
            return;
        }

        // 判断是否存在集群客户端
        auto kfclusterclient = _kf_cluster_client.Find( servername );
        if ( kfclusterclient == nullptr )
        {
            // 判断配置是否存在
            auto kfsetting = _kf_cluster_config->FindClusterSetting( servername );
            if ( kfsetting == nullptr )
            {
                return;
            }

            // 添加进列表
            kfclusterclient = _kf_cluster_client.Create( servername );
            kfclusterclient->_cluster_setting = *kfsetting;
            kfclusterclient->_cluster_client_module = this;
        }

        kfclusterclient->OnConnectionClusterServer( servername, servertype, serverid );
    }

    __KF_CLIENT_LOST_FUNCTION__( KFClusterClientModule::OnClientLostServer )
    {
        auto kfclusterclient = _kf_cluster_client.Find( servername );
        if ( kfclusterclient == nullptr )
        {
            return;
        }

        kfclusterclient->OnLostClusterServer( servertype, serverid );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterAuthAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthAck );

        if ( kfmsg.ip().empty() )
        {
            return;
        }

        auto kfclusterclient = _kf_cluster_client.Find( kfmsg.clustertype() );
        if ( kfclusterclient == nullptr )
        {
            return;
        }

        kfclusterclient->ProcessClusterAuth( kfmsg.name(), kfmsg.type(), kfmsg.id(),  kfmsg.ip(), kfmsg.port(), kfmsg.token() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterClientModule::HandleClusterVerifyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterVerifyAck );

        auto kfclusterclient = _kf_cluster_client.Find( kfmsg.clustertype() );
        if ( kfclusterclient == nullptr )
        {
            return;
        }

        kfclusterclient->ProcessClusterVerify( kfmsg.serverid() );
    }

    ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    bool KFClusterClientModule::SendNetMessage( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfclusterclient = _kf_cluster_client.Find( name );
        if ( kfclusterclient == nullptr )
        {
            KFLogger::LogSystem( KFLogger::Error, "msgid[%u] can't find cluster client[%s]!",
                                 msgid, name.c_str() );
            return false;
        }

        return kfclusterclient->SendNetMessage( msgid, message );
    }

    bool KFClusterClientModule::SendNetMessage( const std::string& name, uint32 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfclusterclient = _kf_cluster_client.Find( name );
        if ( kfclusterclient == nullptr )
        {
            KFLogger::LogSystem( KFLogger::Error, "msgid[%u] can't find cluster client[%s:%u]!",
                                 msgid, name.c_str(), shardid );
            return false;
        }

        return kfclusterclient->SendNetMessage( shardid, msgid, message );
    }

    bool KFClusterClientModule::SendMessageToShard( const std::string& name, uint32 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        return SendNetMessage( name, shardid, msgid, message );
    }

    bool KFClusterClientModule::SendMessageToShard( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        return SendNetMessage( name, msgid, message );
    }

    bool KFClusterClientModule::SendMessageToObject( const std::string& name, uint64 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        KFMsg::S2SSendToClusterObjectReq req;
        req.set_objectid( objectid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        return SendNetMessage( name, KFMsg::S2S_SEND_TO_CLUSTER_OBJECT_REQ, &req );
    }
}