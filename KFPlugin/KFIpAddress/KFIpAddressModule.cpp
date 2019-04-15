#include "KFIpAddressModule.hpp"
#include "KFIpAddressConfig.hpp"
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
    void KFIpAddressModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_ip_config, false );
    }

    void KFIpAddressModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_ip_config );
    }

    void KFIpAddressModule::AfterLoad()
    {
        auto kfglobal = KFGlobal::Instance();

        kfglobal->_local_ip = GetLocalIp();
        if ( kfglobal->_net_type == KFServerEnum::Internet )
        {
            kfglobal->_interanet_ip = GetInteranetIp();
        }
        else
        {
            kfglobal->_interanet_ip = kfglobal->_local_ip;
        }

        __LOG_INFO__( "localip=[{}], interanetip=[{}]", kfglobal->_local_ip, kfglobal->_interanet_ip );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
            auto address = __KF_SHARE_MEMORY__( __KF_STRING__( port ), 10240 );

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

    const KFIpAddress* KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        return _kf_ip_config->FindIpAddress( appname, apptype, appid );
    }

    void KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist )
    {
        _kf_ip_config->FindIpAddress( appname, apptype, appid, outlist );
    }

    const std::string& KFIpAddressModule::GetInteranetIp()
    {
        static std::string _dns_url = "http://members.3322.org/dyndns/getip";

        if ( _interane_ip.empty() )
        {
            // 获得外网地址
            auto interanetip = _kf_http_client->STGet( _dns_url, _invalid_str );
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

    const std::string& KFIpAddressModule::GetLocalIp()
    {
        if ( _local_ip.empty() )
        {
#if __KF_SYSTEM__ == __KF_WIN__
            _local_ip = GetWinLocalIp();
#else
            _local_ip = GetLinuxLocalIp();
#endif
        }

        return _local_ip;
    }

#if __KF_SYSTEM__ == __KF_WIN__
    std::string KFIpAddressModule::GetWinLocalIp()
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
    std::string KFIpAddressModule::GetLinuxLocalIp()
    {
        auto sd = socket( AF_INET, SOCK_DGRAM, 0 );
        if ( -1 == sd )
        {
            return _invalid_str;
        }

        static const char* _keys[] = { "eth0", "enp3s0" };
        static uint32 _count = std::extent<decltype( _keys )>::value;

        std::string ip = "";
        for ( auto i = 0u; i < _count; ++i )
        {
            struct ifreq ifr;
            strncpy( ifr.ifr_name, _keys[ i ], IFNAMSIZ );
            ifr.ifr_name[ IFNAMSIZ - 1 ] = 0;

            if ( ioctl( sd, SIOCGIFADDR, &ifr ) >= 0 )
            {
                struct sockaddr_in sin;
                memcpy( &sin, &ifr.ifr_addr, sizeof( sin ) );
                ip = inet_ntoa( sin.sin_addr );
                if ( !ip.empty() )
                {
                    break;
                }
            }
        }

        close( sd );
        return ip;
    }
#endif

    const std::string& KFIpAddressModule::GetAuthUrl()
    {
        return _kf_ip_config->_auth_url;
    }

    const std::string& KFIpAddressModule::GetLogUrl()
    {
        return _kf_ip_config->_log_url;
    }
}
