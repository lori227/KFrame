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
    void KFIpAddressModule::InitModule()
    {
        // 计算ip地址
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

    const KFIpAddress* KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid )
    {
        return KFIpConfig::Instance()->FindIpAddress( appname, apptype, appid );
    }

    void KFIpAddressModule::FindIpAddress( const std::string& appname, const std::string& apptype, const std::string& appid, IpAddressList& outlist )
    {
        KFIpConfig::Instance()->FindIpAddress( appname, apptype, appid, outlist );
    }

    const std::string& KFIpAddressModule::GetInteranetIp()
    {
        static std::string _dns_url = "http://members.3322.org/dyndns/getip";

        if ( _interane_ip.empty() )
        {
            // 获得外网地址
            auto interanetip = _kf_http_client->STGet( _dns_url, _invalid_string );
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

    const std::string& KFIpAddressModule::GetAuthUrl()
    {
        return KFIpConfig::Instance()->_auth_url;
    }

    const std::string& KFIpAddressModule::GetLogUrl()
    {
        return KFIpConfig::Instance()->_log_url;
    }

    const std::string& KFIpAddressModule::GetClusterName()
    {
        return KFIpConfig::Instance()->_cluster_name;
    }

    const std::string& KFIpAddressModule::GetClusterKey()
    {
        return KFIpConfig::Instance()->_cluster_key;
    }
}
