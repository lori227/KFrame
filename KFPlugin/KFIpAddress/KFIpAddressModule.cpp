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
        // 计算ip地址
        auto kfglobal = KFGlobal::Instance();

        if ( kfglobal->_local_ip.empty() )
        {
            kfglobal->_local_ip = GetLocalIp();
        }

        if ( kfglobal->_interanet_ip.empty() )
        {
            if ( kfglobal->_net_type == KFServerEnum::Internet )
            {
                kfglobal->_interanet_ip = GetInteranetIp();
            }
            else
            {
                kfglobal->_interanet_ip = kfglobal->_local_ip;
            }
        }

        __LOG_INFO__( "localip=[{}], interanetip=[{}]", kfglobal->_local_ip, kfglobal->_interanet_ip );

        auto vpndata = KFIpConfig::Instance()->FindVPNIpAddress( kfglobal->_app_name, kfglobal->_app_type, kfglobal->_app_id->GetZoneId() );
        auto vpnip = std::get<0>( vpndata );
        auto vpnport = std::get<1>( vpndata );
        if ( !vpnip.empty() )
        {
            kfglobal->_interanet_ip = vpnip;
            if ( vpnport != 0u )
            {
                kfglobal->_listen_port = vpnport;
            }

            __LOG_INFO__( "use vpn ip=[{}] port=[{}]", kfglobal->_interanet_ip, kfglobal->_listen_port );
        }
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

    void KFIpAddressModule::BeforeShut()
    {
        __UN_TIMER_0__();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFIpAddressModule::OnTimerUpdateMasterIp )
    {
        auto kfglobal = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( updatemasterip );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), kfglobal->_app_name );
        __JSON_SET_VALUE__( kfjson, __STRING__( appid ), kfglobal->_app_id->GetId() );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), kfglobal->_app_id->GetZoneId() );
        __JSON_SET_VALUE__( kfjson, __STRING__( ip ), kfglobal->_local_ip );
        __JSON_SET_VALUE__( kfjson, __STRING__( port ), kfglobal->_listen_port );
        __JSON_SET_VALUE__( kfjson, __STRING__( time ), 70u );	// 失效时间70秒
        _kf_http_client->MTGet<KFIpAddressModule>( _url, kfjson );
    }

    const KFIpAddress* KFIpAddressModule::GetMasterIp( const std::string& appname, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( querymasterip );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), appname );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), zoneid );

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

        static KFIpAddress _ip_address;
        _ip_address._id = appid;
        _ip_address._ip = ip;
        _ip_address._port = port;
        _ip_address._name = appname;
        _ip_address._type = __STRING__( master );
        _ip_address._port_type = __FIX_PORT__;
        return &_ip_address;
    }

    const std::list< KFIpAddress>& KFIpAddressModule::GetMasterList( const std::string& appname, uint32 zoneid )
    {
        auto kfglobal = KFGlobal::Instance();
        static auto _url = GetDirUrl() + __STRING__( querymasterlist );

        static std::list< KFIpAddress > _ip_address_list;
        _ip_address_list.clear();

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( appname ), appname );
        __JSON_SET_VALUE__( kfjson, __STRING__( zoneid ), zoneid );

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

                KFIpAddress ipaddress;
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

    const std::string& KFIpAddressModule::GetInteranetIp()
    {
        if ( _interane_ip.empty() )
        {
            // 获得外网地址
            auto interanetip = _kf_http_client->STGet( KFIpConfig::Instance()->_dns_get_ip_url, _invalid_string );
            if ( interanetip.empty() )
            {
                // 获得内网地址
                _interane_ip = GetLocalIp();
            }
            else
            {
                _interane_ip = KFUtility::SplitString( interanetip, "\n" );
            }
        }

        return _interane_ip;
    }

    static std::string _default_ip = "127.0.0.1";
    const std::string& KFIpAddressModule::GetLocalIp()
    {
        if ( _local_ip.empty() )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            _local_ip = GetWinLocalIp();
#else
            _local_ip = GetLinuxLocalIp();
#endif
            if ( _local_ip.empty() )
            {
                _local_ip = _default_ip;
            }
        }

        return _local_ip;
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
        return KFIpConfig::Instance()->_log_url;
    }

    const std::string& KFIpAddressModule::GetAuthUrl()
    {
        return KFIpConfig::Instance()->_auth_url;
    }

    const std::string& KFIpAddressModule::GetDirUrl()
    {
        return KFIpConfig::Instance()->_dir_url;
    }

    const std::string& KFIpAddressModule::GetPayUrl()
    {
        return KFIpConfig::Instance()->_pay_url;
    }

}
