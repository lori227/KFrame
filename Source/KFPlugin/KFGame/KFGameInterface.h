#ifndef __KF_GAME_INTERFACE_H__
#define __KF_GAME_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	class KFGameInterface : public KFModule
	{
	public:
		// 发送消息到世界服务器
		virtual bool SendMessageToWorld( uint32 msgid, ::google::protobuf::Message* message ) = 0;

		// 发送消息到Gate服务器
		virtual void SendMessageToGate( uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToGate( uint32 gateid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
		
		// 发送消息到客户端
		virtual bool SendMessageToClient( uint32 gateid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToClient( uint32 gateid, uint32 playerid, uint32 msgid, const char* data, uint32 length ) = 0;

		// 广播消息到客户端
		virtual bool BroadcastMessageToClient( uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool BroadcastMessageToWorld( uint32 msgid, ::google::protobuf::Message* message ) = 0;

		// 消息转发
		virtual bool SendMessageToTransmit( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_game, KFGameInterface );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif