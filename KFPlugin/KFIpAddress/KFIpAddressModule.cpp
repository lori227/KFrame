#include "KFIpAddressModule.hpp"
#include "KFPort.hpp"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <winsock2.h>
    #include <windows.h>
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
    void KFIpAddressModule::AfterLoad()
    {
        // 初始化内网ip地址
        InitLocalIp();
        // 初始化外网ip地址
        InitInteranetIp();
    }

    void KFIpAddressModule::BeforeRun()
    {
        if ( KFGlobal::Instance()->_app_type != __STRING__( master ) )
        {
            return;
        }

        // 启动定时器, 更新master数据( 60秒更新一次 )
        __LOOP_TIMER_0__( 60000u, 1u, &KFIpAddressModule::OnTimerUpdateMasterIp );
    }

    void KFIpAddressModule::OnceRun()
    {
        // 初始化vpn地址( 只有zone.gate需要处理 )
        auto global = KFGlobal::Instance();
        if ( global->_app_name == __STRING__( zone ) && global->_app_type == __STRING__( gate ) )
        {
            auto value = global->GetString( __STRING__( vpn ), global->_app_id->GetZoneId() );
            if ( !value.empty() )
            {
                global->_intranet_ip = KFUtility::SplitString( value, __DOMAIN_STRING__ );
                auto port = KFUtility::SplitValue<uint32>( value, __DOMAIN_STRING__ );
                if ( port != 0u )
                {
                    global->_listen_port = port;
                }
                __LOG_INFO__( "use vpn ip=[{}] port=[{}]", global->_intranet_ip, global->_listen_port );
            }
        }
    }

    void KFIpAddressModule::BeforeShut()
    {
        __UN_TIMER_0__();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFIpAddressModule::OnTimerUpdateMasterIp )
    {
        auto global = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( updatemasterip );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), global->_app_name );
        __JSON_SET_VALUE__( kfjson, __STRING__( appid ), global->_app_id->GetId() );
        __JSON_SET_VALUE__( kfjson, __STRING__( zone_id ), global->_app_id->GetZoneId() );
        __JSON_SET_VALUE__( kfjson, __STRING__( ip ), global->_intranet_ip );
        __JSON_SET_VALUE__( kfjson, __STRING__( port ), global->_listen_port );
        __JSON_SET_VALUE__( kfjson, __STRING__( time ), 70u );	// 失效时间70秒
        _kf_http_client->MTGet<KFIpAddressModule>( _url, kfjson );
    }

    const KFNetData* KFIpAddressModule::GetMasterIp( const std::string& appname, uint32 zone_id )
    {
        auto global = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( querymasterip );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), appname );
        __JSON_SET_VALUE__( kfjson, __STRING__( zone_id ), zone_id );

        auto recvdata = _kf_http_client->STGet( _url, kfjson );
        __JSON_PARSE_STRING__( kfresult, recvdata );
        auto retcode = _kf_http_client->GetCode( kfresult );
        if ( retcode != KFEnum::Ok )
        {
            return nullptr;
        }

        auto ip = __JSON_GET_STRING__( kfresult, __STRING__( ip ) );
        auto port = __TO_UINT32__( __JSON_GET_STRING__( kfresult, __STRING__( port ) ) );
        auto appid = __TO_UINT64__( __JSON_GET_STRING__( kfresult, __STRING__( appid ) ) );
        if ( ip.empty() || port == 0u || appid == 0u )
        {
            return nullptr;
        }

        static KFNetData _ip_address;
        _ip_address._id = appid;
        _ip_address._ip = ip;
        _ip_address._port = port;
        _ip_address._name = appname;
        _ip_address._type = __STRING__( master );
        _ip_address._port_type = __FIX_PORT__;
        return &_ip_address;
    }

    const std::list<KFNetData>& KFIpAddressModule::GetMasterList( const std::string& appname, uint32 zone_id )
    {
        auto global = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( querymasterlist );

        static std::list< KFNetData > _ip_address_list;
        _ip_address_list.clear();

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), appname );
        __JSON_SET_VALUE__( kfjson, __STRING__( zone_id ), zone_id );

        auto recvdata = _kf_http_client->STGet( _url, kfjson );
        __JSON_PARSE_STRING__( kfresult, recvdata );
        auto retcode = _kf_http_client->GetCode( kfresult );
        if ( retcode == KFEnum::Ok )
        {
            auto& kfmasterlist = __JSON_GET_OBJECT__( kfresult, __STRING__( masterlist ) );
            auto size = __JSON_ARRAY_SIZE__( kfmasterlist );
            for ( auto i = 0u; i < size; ++i )
            {
                auto& jsonobject = __JSON_ARRAY_INDEX__( kfmasterlist, i );

                KFNetData ipaddress;
                ipaddress._id = __TO_UINT64__( __JSON_GET_STRING__( jsonobject, __STRING__( appid ) ) );
                ipaddress._ip = __JSON_GET_STRING__( jsonobject, __STRING__( ip ) );
                ipaddress._port = __TO_UINT32__( __JSON_GET_STRING__( jsonobject, __STRING__( port ) ) );
                ipaddress._name = appname;
                ipaddress._type = __STRING__( master );
                ipaddress._port_type = __FIX_PORT__;
                _ip_address_list.emplace_back( ipaddress );
            }
        }

        return _ip_address_list;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFIpAddressModule::CalcListenPort( uint32 type, uint32 port, uint64 appid )
    {
        switch ( type )
        {
        case __ID_PORT__:
        {
            KFAppId kfappid( appid );
            port = port + kfappid.GetWorkId();
        }
        break;
        case __SH_PORT__:
        {
            // 共享内存结算
            auto address = __KF_SHARE_MEMORY__( __STRING__( port ), 10240 );

            auto kframeport = reinterpret_cast< KFramePort* >( address );
            if ( kframeport->_is_inited == 0 )
            {
                kframeport = new ( address ) KFramePort();
                kframeport->_is_inited = 1;
            }

            // 查找自己是否在列表中
            auto kfport = kframeport->FindPort( appid );
            port = port + kfport->_port;
        }
        break;
        default:
            break;
        }

        return port;
    }

    void KFIpAddressModule::InitInteranetIp()
    {
        auto global = KFGlobal::Instance();
        if ( global->_net_type == KFServerEnum::Local )
        {
            global->_intranet_ip = global->_local_ip;
        }
        else
        {
            auto& dnsurl = global->GetString( __STRING__( dnsurl ) );
            do
            {
                // 获得外网地址
                auto interanetip = _kf_http_client->STGet( dnsurl, _invalid_string );
                if ( !interanetip.empty() )
                {
                    global->_intranet_ip = KFUtility::SplitString( interanetip, "\n" );
                }
            } while ( global->_intranet_ip.empty() );
        }

        __LOG_INFO__( "interanetip=[{}]", global->_intranet_ip );
    }

    static std::string _default_ip = "127.0.0.1";
    void KFIpAddressModule::InitLocalIp()
    {
        auto global = KFGlobal::Instance();

#if __KF_SYSTEM__ == __KF_WIN__
        global->_local_ip = GetWinLocalIp();
#else
        global->_local_ip = GetLinuxLocalIp();
#endif
        if ( global->_local_ip.empty() )
        {
            global->_local_ip = _default_ip;
        }

        // 初始化外网ip
        __LOG_INFO__( "localip=[{}]", global->_local_ip );
    }

#if __KF_SYSTEM__ == __KF_WIN__
    std::string KFIpAddressModule::GetWinLocalIp()
    {
        WSADATA wsadata = { 0 };
        if ( WSAStartup( MAKEWORD( 2, 1 ), &wsadata ) != 0 )
        {
            return _invalid_string;
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
    std::string KFIpAddressModule::GetLinuxLocalIp()
    {
        auto sd = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( sd < 0 )
        {
            return _invalid_string;
        }

        uint8 buff[ 1024 ] = { 0 };

        struct ifconf ifc;
        ifc.ifc_buf = ( caddr_t )buff;
        ifc.ifc_len = sizeof( buff );

        // 获得所有接口信息
        ioctl( sd, SIOCGIFCONF, &ifc );

        auto ifr = ( struct ifreq* )buff;
        auto size = ifc.ifc_len / sizeof( struct ifreq );

        std::string ip = "";
        for ( auto i = size; i > 0; --i )
        {
            std::string addr = inet_ntoa( ( ( struct sockaddr_in* ) & ( ifr->ifr_addr ) )->sin_addr );
            if ( !addr.empty() && addr != _default_ip )
            {
                __LOG_INFO__( "name=[{}] ip=[{}]", ifr->ifr_name, addr );
                ip = addr;
                break;
            }

            ++ifr;
        }

        close( sd );
        return ip;
    }
#endif

    const std::string& KFIpAddressModule::GetLogUrl()
    {
        return KFGlobal::Instance()->GetString( __STRING__( logurl ) );
    }

    const std::string& KFIpAddressModule::GetAuthUrl()
    {
        return KFGlobal::Instance()->GetString( __STRING__( authurl ) );
    }

    const std::string& KFIpAddressModule::GetDirUrl()
    {
        return KFGlobal::Instance()->GetString( __STRING__( dirurl ) );
    }

    const std::string& KFIpAddressModule::GetPayUrl()
    {
        return KFGlobal::Instance()->GetString( __STRING__( payurl ) );
    }
}
