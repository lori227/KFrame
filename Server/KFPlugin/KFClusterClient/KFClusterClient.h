#ifndef __KF_CLUSTER_CLIENT_H__
#define __KF_CLUSTER_CLIENT_H__

#include "KFrame.h"
#include "KFTimer/KFTimerInterface.h"

namespace KFrame
{
	class KFClusterClientModule;
	class ClusterClientSetting;
	__ST_CLASS__( KFClusterClient )
	{
	public:
		KFClusterClient();
		~KFClusterClient();
		
		// 开启集群客户端
		void StartClusterClient( KFClusterClientModule* module, const KFConnection* setting );

		// 连接事件
		void OnConnectionClusterServer( const std::string& servertype, uint32 serverid );

		// 断线事件
		void OnLostClusterServer( const std::string& servertype, uint32 serverid );

		// 认证成功
		void ProcessClusterAuth( const std::string& servertype, uint32 serverid, const std::string& name, const std::string& ip, uint32 port, const std::string& token );

		// 验证token成功
		void ProcessClusterVerify( uint32 serverid );

		// 发送消息
		bool SendNetMessage( uint32 msgid, google::protobuf::Message* message );
		bool SendNetMessage( uint32 objectid, uint32 msgid, google::protobuf::Message* message );

		bool SendNetMessage( uint32 msgid, const char* data, uint32 length );
		bool SendNetMessage( uint32 objectid, uint32 msgid, const char* data, uint32 length );

	protected:
		// 发送认证消息
		__KF_TIMER_FUNCTION__( OnTimerSendClusterAuthMessage );

		// 发送Token消息
		__KF_TIMER_FUNCTION__( OnTimerSendClusterTokenMessage );

	private:
		// 模块
		KFClusterClientModule* _cluster_client_module;

		// 集群设置
		KFConnection _cluster_setting;

		// token
		std::string _auth_token;

		// server node type
		std::string _cluster_node_type;

		// server id
		uint32 _cluster_node_id;

		// 是否可以服务
		bool _cluster_in_services;
	};
}



#endif