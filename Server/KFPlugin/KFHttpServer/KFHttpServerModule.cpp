#include "KFHttpServerModule.h"
#include "KFHttpServerConfig.h"
#include "KFHttp/KFHttpServer.h"
#include "KFHttp/KFHttpCommon.h"

#if __KF_SYSTEM__ == __KF_WIN__
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
	KFHttpServerModule::KFHttpServerModule()
	{
		_http_server = __KF_NEW__( KFHttpServer );
	}

	KFHttpServerModule::~KFHttpServerModule()
	{
		__KF_DELETE__( _http_server );
	}

	void KFHttpServerModule::InitModule()
	{
		//////////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_http_server_config, _kf_plugin->_plugin_name, _kf_plugin->_config, false );
	}

	void KFHttpServerModule::AfterLoad()
	{
		auto kfglogal = KFGlobal::Instance();

		auto port = _http_port.CalcListenPort( kfglogal->_app_name, kfglogal->_app_id, _kf_http_server_config->_port );
		_kf_http_server_config->_port = port;
	}

	void KFHttpServerModule::BeforeRun()
	{
		__REGISTER_RUN_FUNCTION__( &KFHttpServerModule::Run );

		_http_server->Start( _kf_http_server_config->_local_ip, _kf_http_server_config->_port, _kf_http_server_config->_max_thread,
			_kf_http_server_config->_max_queue, _kf_http_server_config->_idle_time, _kf_http_server_config->_keep_alive );

		// 重新获得外网ip
		_kf_http_server_config->_interanet_ip = GetLocalIp();
		_full_url = KFUtility::Format( "http://%s:%u/", _kf_http_server_config->_interanet_ip.c_str(), _kf_http_server_config->_port );
	
		KFLogger::LogInit( KFLogger::Info, "http server[%s] startup ok!", _full_url.c_str() );
	}

	void KFHttpServerModule::Run()
	{
		_http_server->Run();
	}

	void KFHttpServerModule::ShutDown()
	{
		_http_server->ShutDown();
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
		__UNREGISTER_RUN_FUNCTION__();
	}

	std::string KFHttpServerModule::GetLocalIp()
	{
#if __KF_SYSTEM__ == __KF_WIN__
		return GetWinLocalIp();
#else
		return GetLinuxLocalIp();
#endif
	}

#if __KF_SYSTEM__ == __KF_WIN__
	std::string KFHttpServerModule::GetWinLocalIp()
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
	std::string KFHttpServerModule::GetLinuxLocalIp()
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KFHttpServerModule::RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function )
	{
		_http_server->RegisterMethonFunction( url, sync, function );
	}

	void KFHttpServerModule::UnRegisterFunction( const std::string& url )
	{
		_http_server->UnRegisterMethonFunction( url );
	}

	const std::string& KFHttpServerModule::GetHttpUrl()
	{
		return _full_url;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string KFHttpServerModule::SendResponseCode( uint32 code )
	{
		return KFHttpCommon::SendResponseCode( code );
	}

	uint32 KFHttpServerModule::GetResponseCode( KFJson& json )
	{
		return KFHttpCommon::GetResponseCode( json );
	}

	std::string KFHttpServerModule::SendResponse( KFJson& json )
	{
		return KFHttpCommon::SendResponse( json );
	}

	std::string KFHttpServerModule::SendResponse( KFJson& json, uint32 code )
	{
		return KFHttpCommon::SendResponse( json, code );
	}

	void KFHttpServerModule::MakeSignature( KFJson& json )
	{
		KFHttpCommon::MakeSignature( json );
	}

	bool KFHttpServerModule::VerifySignature( KFJson& json )
	{
		return KFHttpCommon::VerifySignature( json );
	}
}