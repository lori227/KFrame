#ifndef __KF_CLUSTER_CLIENT_CONFIG_H__
#define __KF_CLUSTER_CLIENT_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	class KFClusterClientConfig : public KFConfig, public KFSingleton< KFClusterClientConfig >
	{
	public:
		KFClusterClientConfig();
		~KFClusterClientConfig();

		bool LoadConfig( const char* file );
		
	public:
		// 集群客服端配置列表
		std::vector< KFConnection > _cluster_client_setting;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_cluster_client_config = KFClusterClientConfig::Instance();
	//////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif