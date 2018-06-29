#include "KFTcpServerModule.h"
#include "KFTcpServerConfig.h"
#include "KFProtocol/KFProtocol.h"


#if __KF_SYSTEM__ == __KF_WIN__

#else
    #include <unistd.h>
    #include <netdb.h>
    #include <net/if.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <sys/types.h>
    #include <sys/socket.h>
#endif


namespace KFrame
{
    KFTcpServerModule::KFTcpServerModule()
    {
        _kf_server_engine = nullptr;
        _kf_transmit_function = nullptr;
    }

    KFTcpServerModule::~KFTcpServerModule()
    {
        __KF_DELETE__( _kf_server_engine );
    }

    void KFTcpServerModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_server_config, false );
        _kf_server_engine = __KF_NEW__( KFNetServerEngine );
    }

    static std::string _default_ip = "0.0.0.0";
    static std::string _local_ip = "127.0.0.1";
    void KFTcpServerModule::AfterLoad()
    {
        static std::string dnsurl = "http://members.3322.org/dyndns/getip";

        auto listen = &_kf_server_config->_kf_listen;
        if ( listen->_interanet_ip == _default_ip )
        {
            // 获得外网地址
            auto interanetip = _kf_http_client->StartSTHttpClient( dnsurl, _invalid_str );
            if ( interanetip.empty() )
            {
                // 获得内网地址
                interanetip = GetLocalIp();
            }
            else
            {
                interanetip = KFUtility::SplitString( interanetip, "\n" );
            }

            if ( !interanetip.empty() )
            {
                listen->_interanet_ip = interanetip;
            }
            else
            {
                listen->_interanet_ip = _local_ip;
            }
        }
        else if ( listen->_interanet_ip == _local_ip )
        {
            auto localip = GetLocalIp();
            if ( !localip.empty() )
            {
                listen->_interanet_ip = localip;
            }
        }

        auto kfglobal = KFGlobal::Instance();
        KFLogger::LogInit( KFLogger::Info, "[%s:%s] interanet ip : [%s]",
                           kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), listen->_interanet_ip.c_str() );
    }

    std::string KFTcpServerModule::GetLocalIp()
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return GetWinLocalIp();
#else
        return GetLinuxLocalIp();
#endif
    }

#if __KF_SYSTEM__ == __KF_WIN__
    std::string KFTcpServerModule::GetWinLocalIp()
    {
        WSADATA wsadata = { 0 };
        if ( WSAStartup( MAKEWORD( 2, 1 ), &wsadata ) != 0 )
        {
            return _invalid_str;
        }

        std::string ip = "";
        char hostname[ MAX_PATH ] = { 0 };
        auto retcode = gethostname( hostname, sizeof( hostname ) );
        if ( retcode == 0 )
        {
            auto hostinfo = gethostbyname( hostname );
            ip = inet_ntoa( *( struct in_addr* )*hostinfo->h_addr_list );
        }

        WSACleanup();
        return ip;
    }
#else
    std::string KFTcpServerModule::GetLinuxLocalIp()
    {
        auto sd = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( -1 == sd )
        {
            return _invalid_str;
        }

        struct ifreq ifr;
        strncpy( ifr.ifr_name, "eth0", IFNAMSIZ );
        ifr.ifr_name[ IFNAMSIZ - 1 ] = 0;

        std::string ip = "";
        if ( ioctl( sd, SIOCGIFADDR, &ifr ) >= 0 )
        {
            struct sockaddr_in sin;
            memcpy( &sin, &ifr.ifr_addr, sizeof( sin ) );
            ip = inet_ntoa( sin.sin_addr );
        }

        close( sd );
        return ip;
    }
#endif

    void KFTcpServerModule::BeforeRun()
    {
        auto kfglobal = KFGlobal::Instance();
        auto listen = &_kf_server_config->_kf_listen;

        _kf_server_engine->InitEngine( listen->_max_queue_size );

        _kf_server_engine->BindNetFunction( this, &KFTcpServerModule::HandleNetMessage );
        _kf_server_engine->BindLostFunction( this, &KFTcpServerModule::OnServerLostHandle );

        __REGISTER_RUN_FUNCTION__( &KFTcpServerModule::Run );
        ////////////////////////////////////////////////////////////////////////////////////////////

        kfglobal->_local_ip = listen->_local_ip;
        kfglobal->_interanet_ip = listen->_interanet_ip;
        kfglobal->_listen_port = _kf_net_port.CalcListenPort( listen->_type, kfglobal->_app_id, _kf_connection->GetListenPort() );
        auto result = _kf_server_engine->StartEngine( kfglobal->_local_ip, kfglobal->_listen_port );
        if ( result == 0 )
        {
            KFLogger::LogInit( KFLogger::Info, "[%s:%s|%s:%u] tcp services ok!",
                               kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_interanet_ip.c_str(), kfglobal->_listen_port );
        }
        else
        {
            KFLogger::LogInit( KFLogger::Error, " [%s:%s|%s:%u] tcp services failed[ %d ]!",
                               kfglobal->_app_name.c_str(), kfglobal->_app_type.c_str(), kfglobal->_interanet_ip.c_str(), kfglobal->_listen_port, result );
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ, &KFTcpServerModule::HandleRegisterReq );
    }

    void KFTcpServerModule::BeforeShut()
    {
        _kf_config->RemoveConfig( _kf_plugin->_plugin_name );
        ///////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REGISTER_TO_SERVER_REQ );
    }

    void KFTcpServerModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();
        _kf_server_engine->ShutEngine();
    }

    void KFTcpServerModule::Run()
    {
        _kf_server_engine->RunEngine( KFGlobal::Instance()->_game_time );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFTcpServerModule::OnServerLostHandle )
    {
        auto kfhandle = _kf_server_engine->FindNetHandle( handleid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        BroadcastLostFromServer( kfhandle );
        CallLostFunction( kfhandle );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFTcpServerModule::RegisteNetHandle( uint32 sessionid, uint32 objectid )
    {
        auto kfhandle = _kf_server_engine->RegisteHandle( sessionid, objectid );
        if ( kfhandle == nullptr )
        {
            return false;
        }

        return true;
    }

    bool KFTcpServerModule::CloseNetHandle( uint32 handleid, uint32 delaytime, const char* function, uint32 line )
    {
        return _kf_server_engine->CloseHandle( handleid, delaytime, function, line );
    }

    uint32 KFTcpServerModule::GetHandleCount()
    {
        return _kf_server_engine->GetHandleCount();
    }

    void KFTcpServerModule::GetHandleList( std::list<uint32>& handlelist )
    {
        _kf_server_engine->GetHandleList( handlelist );
    }

    const std::string& KFTcpServerModule::GetHandleIp( uint32 handleid )
    {
        return _kf_server_engine->GetHandleIp( handleid );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::AddDiscoverFunction( const char* module, KFServerDiscoverFunction& function )
    {
        auto kffunction = _kf_discover_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveDiscoverFunction( const char* module )
    {
        _kf_discover_function.Remove( module );
    }

    void KFTcpServerModule::CallDiscoverFunction( KFNetHandle* tcphandle )
    {
        for ( auto& iter : _kf_discover_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( tcphandle->_id, tcphandle->_app_name, tcphandle->_app_type, tcphandle->_listen_ip, tcphandle->_listen_port );
        }
    }

    void KFTcpServerModule::AddLostFunction( const char* module, KFServerLostFunction& function )
    {
        auto kffunction = _kf_lost_function.Create( module );
        kffunction->_function = function;
    }

    void KFTcpServerModule::RemoveLostFunction( const char* module )
    {
        _kf_lost_function.Remove( module );
    }

    void KFTcpServerModule::CallLostFunction( KFNetHandle* tcphandle )
    {
        for ( auto& iter : _kf_lost_function._objects )
        {
            auto kffunction = iter.second;
            kffunction->_function( tcphandle->_id, tcphandle->_app_name, tcphandle->_app_type );
        }
    }

    void KFTcpServerModule::AddTransmitFunction( const char* module, KFTransmitFunction& function )
    {
        _kf_transmit_function = function;
    }

    void KFTcpServerModule::RemoveTransmitFunction( const char* module )
    {
        _kf_transmit_function = nullptr;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFTcpServerModule::HandleNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        bool result = _kf_message->CallFunction( kfguid, msgid, data, length );
        if ( result )
        {
            return;
        }

        if ( _kf_transmit_function != nullptr )
        {
            auto ok = _kf_transmit_function( kfguid, msgid, data, length );
            if ( !ok )
            {
                KFLogger::LogNet( KFLogger::Error, "[%s] tcp server transmit msgid[%u] failed!",
                                  __FUNCTION__, msgid );
            }
        }
        else
        {
            KFLogger::LogSystem( KFLogger::Error, "msgid[%u] can't find function!", msgid );
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFTcpServerModule::HandleRegisterReq )
    {
        __PROTO_PARSE__( KFMsg::RegisterToServerReq );
        auto sessionid = __KF_HEAD_ID__( kfguid );

        auto listendata = &kfmsg.listen();
        auto handlid = listendata->appid();
        auto name = listendata->appname();
        auto type = listendata->apptype();

        auto kfhandle = _kf_server_engine->RegisteHandle( sessionid, handlid );
        if ( kfhandle == nullptr )
        {
            return;
        }

        kfhandle->_app_name = name;
        kfhandle->_app_type = type;
        kfhandle->_app_id = listendata->appid();
        kfhandle->_listen_ip = listendata->ip();
        kfhandle->_listen_port = listendata->port();

        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto kfglobal = KFGlobal::Instance();

        // 注册回馈
        KFMsg::RegisterToServerAck ack;
        ack.set_apptype( kfglobal->_app_type );
        ack.set_appid( kfglobal->_app_id );
        ack.set_appname( kfglobal->_app_name );
        SendNetMessage( handlid, KFMsg::S2S_REGISTER_TO_SERVER_ACK, &ack );

        // 广播新连接给所有连接着
        BroadcastRegisterToServer( kfhandle );

        // 注册信息发送个给新连接
        SendRegisterToServerToHandle( kfhandle );

        CallDiscoverFunction( kfhandle );
        KFLogger::LogSystem( KFLogger::Info, "[%s:%s:%u|%s:%u] register ok!",
                             name.c_str(), type.c_str(), handlid, listendata->ip().c_str(), listendata->port() );
    }

    void KFTcpServerModule::BroadcastRegisterToServer( KFNetHandle* kfhandle )
    {
        KFMsg::TellRegisterToServer tell;
        auto listendata = tell.mutable_listen();

        listendata->set_appid( kfhandle->_app_id );
        listendata->set_appname( kfhandle->_app_name );
        listendata->set_apptype( kfhandle->_app_type );
        listendata->set_ip( kfhandle->_listen_ip );
        listendata->set_port( kfhandle->_listen_port );
        SendNetMessage( KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell, kfhandle->_id );
    }

    void KFTcpServerModule::SendRegisterToServerToHandle( KFNetHandle* kfhandle )
    {
        auto nethandle = _kf_server_engine->_kf_handles.First();
        while ( nethandle != nullptr )
        {
            if ( nethandle->_id != kfhandle->_id )
            {
                KFMsg::TellRegisterToServer tell;
                auto listendata = tell.mutable_listen();

                listendata->set_appid( nethandle->_app_id );
                listendata->set_appname( nethandle->_app_name );
                listendata->set_apptype( nethandle->_app_type );
                listendata->set_ip( nethandle->_listen_ip );
                listendata->set_port( nethandle->_listen_port );
                SendNetMessage( kfhandle->_id, KFMsg::S2S_TELL_REGISTER_TO_SERVER, &tell );
            }

            nethandle = _kf_server_engine->_kf_handles.Next();
        }
    }

    void KFTcpServerModule::BroadcastLostFromServer( KFNetHandle* kfhandle )
    {
        if ( !kfhandle->IsServer() )
        {
            return;
        }

        KFMsg::TellUnRegisterFromServer tell;
        tell.set_appid( kfhandle->_id );
        SendNetMessage( KFMsg::S2S_TELL_UNREGISTER_FROM_SERVER, &tell, kfhandle->_id );

        KFLogger::LogSystem( KFLogger::Error, "[%s:%s:%u|%s:%u] lost connect!",
                             kfhandle->_app_name.c_str(), kfhandle->_app_type.c_str(), kfhandle->_id, kfhandle->_listen_ip.c_str(), kfhandle->_listen_port );
    }

    void KFTcpServerModule::SendNetMessage( const std::string& name, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        _kf_server_engine->SendNetMessage( name, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, google::protobuf::Message* message, uint32 excludeid /* = 0 */ )
    {
        auto strdata = message->SerializeAsString();
        SendNetMessage( msgid, strdata.data(), strdata.size(), excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_server_engine->SendNetMessage( handleid, msgid, strdata.data(), strdata.size() );
    }

    void KFTcpServerModule::SendNetMessage( uint32 msgid, const char* data, uint32 length, uint32 excludeid /* = 0 */ )
    {
        _kf_server_engine->SendNetMessage( msgid, data, length, excludeid );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_server_engine->SendNetMessage( handleid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, const char* data, uint32 length )
    {
        return _kf_server_engine->SendNetMessage( handleid, objectid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( uint32 handleid, uint32 objectid, uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        return _kf_server_engine->SendNetMessage( handleid, objectid, msgid, strdata.data(), strdata.size() );
    }

    bool KFTcpServerModule::SendNetMessage( const KFGuid& kfguid, uint32 msgid, const char* data, uint32 length )
    {
        auto handleid = __KF_HEAD_ID__( kfguid );
        auto objectid = __KF_DATA_ID__( kfguid );
        return SendNetMessage( handleid, objectid, msgid, data, length );
    }

    bool KFTcpServerModule::SendNetMessage( const KFGuid& kfguid, uint32 msgid, google::protobuf::Message* message )
    {
        auto handleid = __KF_HEAD_ID__( kfguid );
        auto objectid = __KF_DATA_ID__( kfguid );
        return SendNetMessage( handleid, objectid, msgid, message );
    }
}