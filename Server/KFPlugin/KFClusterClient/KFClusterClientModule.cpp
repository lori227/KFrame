#include "KFClusterClientModule.h"
#include "KFProtocol/KFProtocol.h"
#include "KFClusterClientConfig.h"

namespace KFrame
{
    void KFClusterClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_cluster_client_config, true );
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
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __KF_REMOVE_CONFIG__( _kf_cluster_client_config );
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

    void KFClusterClientModule::CallClusterConnectionFunction( const std::string& name, uint64 serverid )
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

        auto kfsetting = _kf_cluster_client_config->FindClusterSetting( servername );
        if ( kfsetting == nullptr )
        {
            return;
        }

        // 判断是否存在集群客户端
        auto kfclusterclient = _kf_cluster_client.Find( servername );
        if ( kfclusterclient == nullptr )
        {
            // 添加进列表
            kfclusterclient = _kf_cluster_client.Create( servername );
            kfclusterclient->_cluster_client_module = this;
            kfclusterclient->_kf_cluster_setting = kfsetting;
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
            __LOG_ERROR__( "msgid[{}] can't find cluster client[{}]!", msgid, name );
            return false;
        }

        return kfclusterclient->SendNetMessage( msgid, message );
    }

    bool KFClusterClientModule::SendNetMessage( const std::string& name, uint64 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        auto kfclusterclient = _kf_cluster_client.Find( name );
        if ( kfclusterclient == nullptr )
        {
            __LOG_ERROR__( "msgid[{}] can't find cluster client[{}:{}]!", msgid, name, KFAppID::ToString( shardid ) );
            return false;
        }

        return kfclusterclient->SendNetMessage( shardid, msgid, message );
    }

    bool KFClusterClientModule::SendToShard( const std::string& name, uint64 shardid, uint32 msgid, google::protobuf::Message* message )
    {
        return SendNetMessage( name, shardid, msgid, message );
    }

    bool KFClusterClientModule::SendToShard( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        return SendNetMessage( name, msgid, message );
    }

    bool KFClusterClientModule::SendToStaticObject( const std::string& name, uint64 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        KFMsg::S2SSendToStaticObjectReq req;
        req.set_objectid( objectid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_serverid( KFGlobal::Instance()->_app_id._union._id );
        return SendNetMessage( name, KFMsg::S2S_SEND_TO_STATIC_OBJECT_REQ, &req );
    }

    bool KFClusterClientModule::SendToDynamicObject( const std::string& name, uint64 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        KFMsg::S2SSendToDynamicObjectReq req;
        req.set_objectid( objectid );
        req.set_msgid( msgid );
        req.set_msgdata( message->SerializeAsString() );
        req.set_serverid( KFGlobal::Instance()->_app_id._union._id );
        return SendNetMessage( name, KFMsg::S2S_SEND_TO_DYNAMIC_OBJECT_REQ, &req );
    }
}