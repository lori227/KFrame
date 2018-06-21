#ifndef __KF_RELATION_CLIENT_INTERFACE_H__
#define __KF_RELATION_CLIENT_INTERFACE_H__

#include "KFrame.h"

//http://wiki.msdk.qq.com/Router/server.html#wxfriends_profile
namespace KFrame
{
	class KFData;
	class KFEntity;
	class KFRelationClientInterface : public KFModule
	{
	public:
		// 发送消息到关系集群
		virtual bool SendMessageToRelation( uint32 msgid, ::google::protobuf::Message* message ) = 0;
		
		// 发送消息到关系属性
		virtual bool SendMessageToRelation( KFData* kfrelation, uint32 msgid, google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToRelation( uint32 serverid, uint32 playerid, uint32 msgid, google::protobuf::Message* message ) = 0;
			
		
		// 添加好友度
		virtual void UpdateFriendLiness( KFEntity* player, uint32 friendid, uint32 operate, uint64 value ,uint32 type) = 0;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_relation, KFRelationClientInterface );
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif