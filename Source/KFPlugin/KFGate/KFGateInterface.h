#ifndef __KF_GATE_INTERFACE_H__
#define __KF_GATE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////////////////
	class KFGateInterface : public KFModule
	{
	public:
		// 是否连接到登录服务器
		virtual bool IsLoginConnected() = 0;

		// 发送消息到登录服务器
		virtual bool SendMessageToLogin( uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToLogin( uint32 accountid, uint32 msgid, ::google::protobuf::Message* message ) = 0;

		// 发送消息到游戏服务器
		virtual bool SendMessageToGame( uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToGame( uint32 gameid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
		virtual bool SendMessageToGame( uint32 gameid, uint32 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
		
		// 发送消息到客户端
		virtual bool SendMessageToClient( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message ) = 0;

		// 添加链接
		virtual bool AddConnection( uint32 connectid, uint32 playerid ) = 0;

		// 删除链接
		virtual bool RemoveConnection( uint32 playerid, uint32 delaytime, const char* function, uint32 line ) = 0;
	
		// 获得ip
		virtual const std::string& GetIp( uint32 connectid ) = 0;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_gate, KFGateInterface );
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif