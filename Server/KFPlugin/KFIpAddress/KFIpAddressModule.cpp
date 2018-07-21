#include "KFIpAddressModule.h"
#include "KFIpAddressConfig.h"


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
    KFIpAddressModule::KFIpAddressModule()
    {
        _tcp_net_port.InitMemoryType( "tcp" );
        _http_net_port.InitMemoryType( "http" );
    }

    KFIpAddressModule::~KFIpAddressModule()
    {

    }

    void KFIpAddressModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_ip_config, true );
    }


    void KFIpAddressModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFIpAddressModule::CalcTcpListenPort( uint32 type, uint32 port, uint32 appid )
    {
        return _tcp_net_port.CalcListenPort( type, port, appid );
    }

    uint32 KFIpAddressModule::CalcHttpListenPort( uint32 type, uint32 port, uint32 appid )
    {
        return _http_net_port.CalcListenPort( type, port, appid );
    }

    const std::string& KFIpAddressModule::FindPlatformAddress( uint32 id )
    {
        return _kf_ip_config->FindPlatformAddress( id );
    }

    const KFIpAddress* KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid )
    {
        return _kf_ip_config->FindIpAddress( appname, apptype, appid );
    }

    void KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, uint32 appid, IpAddressList& outlist )
    {
        _kf_ip_config->FindIpAddress( appname, apptype, appid, outlist );
    }

    void KFIpAddressModule::SetZoneIpAddress( uint32 zoneid, const std::string& ip )
    {
        _kf_ip_config->SetZoneIpAddress( zoneid, ip );
    }

    static std::string _default_local_ip = "127.0.0.1";
    static std::string _default_interanet_ip = "0.0.0.0";
    const std::string& KFIpAddressModule::CalcIpAddress( const std::string& ip )
    {
        // 计算ip
        if ( ip == _default_interanet_ip )
        {
            // 获得外网地址
            return GetInteranetIp();
        }
        else if ( ip == _default_local_ip )
        {
            return GetLocalIp();
        }

        return ip;
    }

    const std::string& KFIpAddressModule::GetInteranetIp()
    {
        static std::string _dns_url = "http://members.3322.org/dyndns/getip";

        if ( _interane_ip.empty() )
        {
            // 获得外网地址
            auto interanetip = _kf_http_client->StartSTHttpClient( _dns_url, _invalid_str );
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
}
