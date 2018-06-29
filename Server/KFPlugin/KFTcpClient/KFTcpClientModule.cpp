#include "KFTcpClientModule.h"
#include "KFTcpClientConfig.h"
#include "KFProtocol/KFProtocol.h"



namespace KFrame
{
    KFTcpClientModule::KFTcpClientModule()
    {
        _kf_client_engine = nullptr;
        _kf_transmit_function = nullptr;
    }

    KFTcpClientModule::~KFTcpClientModule()
    {
        __KF_DELETE__( _kf_client_engine );
    }

    void KFTcpClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_client_config, false );
        _kf_client_engine = __KF_NEW__( KFNetClientEngine );
    }

    void KFTcpClientModule::BeforeRun()
    {
        __REGISTER_RUN_FUNCTION__( &KFTcpClientModule::Run );

        _kf_client_engine->InitEngine( _kf_client_config->_max_queue_size );

        _kf_client_engine->BindNetFunction( this, &KFTcpClientModule::HandleNetMessage );
        _kf_client_engine->BindConnectFunction( this, &KFTcpClientModule::OnClientConnected );
        _kf_client_engine->BindDisconnectFunction( this, &KFTcpClientModule::OnClientDisconnect );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // 启动网络
        auto kfconnection = _kf_connection->FindMasterConnection( _kf_client_config->_kf_master_connection._name );
        if ( kfconnection != nullptr )
        {
            StartNetClient( kfconnection );
        }

        //////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_ACK, &KFTcpClientModule::HandleRegisterAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &KFTcpClientModule::HanldeTellRegisterToServer );
        __REGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &KFTcpClientModule::HanldeTellUnRegisterFromServer );
    }

    void KFTcpClientModule::BeforeShut()
    {
        ////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_REGISTER_TO_SERVER );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER );
    }

    void KFTcpClientModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();

        _kf_client_engine->ShutEngine();
    }

    void KFTcpClientModule::Run()
    {
        // 网络事件
        _kf_client_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////
    // 连接成功
    __KF_CLIENT_CONNECT_FUNCTION__( KFTcpClientModule::OnClientConnected )
    {
        // 把自己注册到服务器
        KFMsg::RegisterToServerReq req;
        auto listendata = req.mutable_listen();

        auto kfglobal = KFGlobal::Instance();

        listendata->set_appid( kfglobal->_app_id );
        listendata->set_apptype( kfglobal->_app_type );
        listendata->set_appname( kfglobal->_app_name );
        listendata->set_ip( kfglobal->_interanet_ip );
        listendata->set_port( kfglobal->_listen_port );

        auto strdata = req.SerializeAsString();
        SendNetMessage( serverid, KFMsg::S2S_REGISTER_TO_SERVER_REQ, strdata.data(), strdata.size() );
    }

    // 连接断开
    __KF_CLIENT_LOST_FUNCTION__( KFTcpClientModule::OnClientDisconnect )
    {
        CallClientLostFunction( serverid, servername, servertype );
    }

    void KFTcpClientModule::StartNetClient( const KFConnection* connection )
    {
        if ( !connection->IsValid() )
        {
            return;
        }

        auto port = connection->_port;
        if ( port == 0 )
        {
            port = _kf_connection->GetListenPort() + connection->_id;
        }

        _kf_client_engine->StartClient( connection->_type, connection->_id, connection->_name, connection->_ip, port );
    }

    void KFTcpClientModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _kf_client_engine->SendNetMessage( msgid, strdata.data(), strdata.size() );
    }

    bool KFTcpClientModule::SendNetMessage( uint32 serverid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_client_engine->SendNetMessage( serverid, msgid, strdata.data(), strdata.size() );
    }

    bool KFTcpClientModule::SendNetMessage( uint32 serverid, uint32 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_client_engine->SendNetMessage( serverid, objectid, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendNetMessage( uint32 msgid, const char* data, uint32 length )
    {
        _kf_client_engine->SendNetMessage( msgid, data, length );
    }

    bool KFTcpClientModule::SendNetMessage( uint32 serverid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_client_engine->SendNetMessage( serverid, msgid, data, length );
    }

    bool KFTcpClientModule::SendNetMessage( uint32 serverid, uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_client_engine->SendNetMessage( serverid, objectid, msgid, data, length );
    }

    // 给某一类型服务器发送消息
    void KFTcpClientModule::SendMessageToType( const std::string& servertype, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToType( servertype, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToType( const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        _kf_client_engine->SendMessageToType( servertype, msgid, data, length );
    }

    void KFTcpClientModule::SendMessageToName( const std::string& servername, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToName( servername, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToName( const std::string& servername, uint32 msgid, const char* data, uint32 length )
    {
        _kf_client_engine->SendMessageToName( servername, msgid, data, length );
    }

    void KFTcpClientModule::SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        SendMessageToServer( servername, servertype, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpClientModule::SendMessageToServer( const std::string& servername, const std::string& servertype, uint32 msgid, const char* data, uint32 length )
    {
        _kf_client_engine->SendMessageToServer( servername, servertype, msgid, data, length );
    }

    void KFTcpClientModule::HandleNetMessage( const KFGuid& guid, uint32 msgid, const char* data, uint32 length )
    {
        bool handleresult = _kf_message->CallFunction( guid, msgid, data, length );
        if ( handleresult )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( guid, msgid, data, length );
            if ( !ok )
            {
                KFLogger::LogNet( KFLogger::Error, "[%s] tcp client transmit msgid[%u] failed!",
                                  __FUNCTION__, msgid );
            }
        }
        else
        {
            KFLogger::LogSystem( KFLogger::Error, "msgid[%u] can't find function!", msgid );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpClientModule::CloseClient( uint32 serverid )
    {
        _kf_client_engine->CloseClient( serverid );
    }

    void KFTcpClientModule::AddNeedConnection( const std::string& servername, const std::string& servertype )
    {
        KFConnection connection;
        connection._type = servertype;
        connection._name = servername;
        _kf_client_config->_kf_connections.push_back( connection );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpClientModule::AddConnectionFunction( const char* name, KFClientConnectionFunction& function )
    {
        auto kffunction = _kf_connection_function.Create( name );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveConnectionFunction( const char* name )
    {
        _kf_connection_function.Remove( name );
    }

    void KFTcpClientModule::CallClientConnectionFunction( uint32 serverid, const std::string& servername, const std::string& servertype )
    {
        for ( auto& iter : _kf_connection_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( serverid, servername, servertype );
        }
    }

    void KFTcpClientModule::AddLostFunction( const char* name, KFClientLostFunction& function )
    {
        auto kffunction = _kf_lost_function.Create( name );
        kffunction->_function = function;
    }

    void KFTcpClientModule::RemoveLostFunction( const char* name )
    {
        _kf_lost_function.Remove( name );
    }

    void KFTcpClientModule::CallClientLostFunction( uint32 serverid, const std::string& servertype, const std::string& servername )
    {
        for ( auto& iter : _kf_lost_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( serverid, servername, servertype );
        }
    }

    void KFTcpClientModule::RemoveTransmitFunction( const char* name )
    {
        _kf_transmit_function = nullptr;
    }

    void KFTcpClientModule::AddTransmitFunction( const char* name, KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpClientModule::StartClient( const std::string& servertype, uint32 serverid, const std::string& name, const std::string& ip, uint32 port )
    {
        if ( port == 0 )
        {
            port = _kf_connection->GetListenPort() + serverid;
        }

        _kf_client_engine->StartClient( servertype, serverid, name, ip, port );
    }

    void KFTcpClientModule::StartNetConnection( const KFMsg::ListenData* listendata )
    {
        if ( !_kf_client_config->NeedToConnection( listendata->appname(), listendata->apptype(), listendata->appid() ) )
        {
            return;
        }

        _kf_client_engine->StartClient( listendata->apptype(), listendata->appid(), listendata->appname(), listendata->ip(), listendata->port() );
    }

    // 通知有客户端注册
    __KF_MESSAGE_FUNCTION__( KFTcpClientModule::HanldeTellRegisterToServer )
    {
        __PROTO_PARSE__( KFMsg::TellRegisterToServer );

        auto listendata = &kfmsg.listen();
        StartNetConnection( listendata );
    }

    __KF_MESSAGE_FUNCTION__( KFTcpClientModule::HanldeTellUnRegisterFromServer )
    {
        __PROTO_PARSE__( KFMsg::TellUnRegisterFromServer );

        CloseClient( kfmsg.appid() );
    }

    // 注册回馈
    __KF_MESSAGE_FUNCTION__( KFTcpClientModule::HandleRegisterAck )
    {
        __PROTO_PARSE__( KFMsg::RegisterToServerAck );

        auto servertype = kfmsg.apptype();
        auto serverid = kfmsg.appid();
        auto& servername = kfmsg.appname();

        // 处理回调函数
        CallClientConnectionFunction( serverid, servername, servertype );

        auto* kfclient = _kf_client_engine->FindClient( serverid );
        if ( kfclient != nullptr )
        {
            auto kfsetting = &kfclient->_net_setting;

            KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u|%s:%u] register ok!",
                                 servername.c_str(), servertype.c_str(), serverid, kfsetting->_ip.c_str(), kfsetting->_port );
        }
    }
}