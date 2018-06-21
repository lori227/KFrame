#ifndef __KF_CONNECTION_CONFIG_H__
#define __KF_CONNECTION_CONFIG_H__

#include "KFrame.h"
#include "KFConHash/KFConHash.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFConnectionConfig : public KFConfig, public KFSingleton< KFConnectionConfig >
	{
	public:
		KFConnectionConfig();
		~KFConnectionConfig();

		bool LoadConfig( const char* file );

		// 查找平台地址
		const std::string& FindPlatformAddress( uint32 id );

		// 查找master连接
		const KFConnection* FindMasterConnection( const std::string& name ) const;

		// 修改mastr连接id
		void SetMasterConnection( const std::string& name, uint32 zoneid );

	public:
		// 默认的监听端口
		uint32 _listen_port;

		// Master连接地址
		std::map< std::string, KFConnection > _master_connection;

		// 平台地址
		KFConHash _platform_hash;
		std::map< uint32, std::string > _platform_address;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_connection_config = KFConnectionConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif