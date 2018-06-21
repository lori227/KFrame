#ifndef __KF_CLUSTER_CLIENT_INTERFACE_H__
#define __KF_CLUSTER_CLIENT_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
	typedef std::function< void( uint32 serverid ) > KFClusterConnectionFunction;
	class KFClusterClientInterface : public KFModule
	{
	public:
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		template< class T >
		void RegisterConnectionFunction( const std::string& name, T* object, void ( T::*handle )( uint32 ) )
		{
			KFClusterConnectionFunction function = std::bind( handle, object, std::placeholders::_1 );
			AddConnectionFunction( name, function );
		}

		void UnRegisterConnectionFunction( const std::string& name )
		{
			RemoveConnectionFunction( name );
		}
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// 发送消息
		virtual bool SendMessageToShard( const std::string& name, uint32 msgid, google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToShard( const std::string& name, uint32 shardid, uint32 msgid, google::protobuf::Message* message ) = 0;
		
		// 发送到对象所在的分片服务器
		virtual bool SendMessageToObject( const std::string& name, uint64 objectid, uint32 msgid, google::protobuf::Message* message ) = 0;

	protected:
		virtual void AddConnectionFunction( const std::string& name, KFClusterConnectionFunction& function ) = 0;
		virtual void RemoveConnectionFunction( const std::string& name ) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_cluster, KFClusterClientInterface );
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif