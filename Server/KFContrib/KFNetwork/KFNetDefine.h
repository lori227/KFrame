#ifndef __NET_DEFINE_H__
#define __NET_DEFINE_H__

#include "KFInclude.h"

namespace KFrame
{
	class KFNetConnector;
	namespace KFNetDefine
	{
		enum EConstDefine
		{
			// mtu = 1500 , data = 1472
			MaxMessageLength = 1024 * 5,
			MaxReqBuffLength = MaxMessageLength + 24,		// 投递给网络底层的bbuff大小
			MaxRecvBuffLength = 2 * MaxReqBuffLength,		// 接收消息的最大长度

			WaitConnectTimeOut = 5,				// 尝试连接间隔时间  单位 : 秒
			WaitSendTimeOut = 1,				// 等待发送消息时间 单位: 毫秒

			ClintPingTime = 30,				// ping 时间, 单位 : 秒
			ServerPingTimeOut = 70,			// ping 超时, 单位 : 秒
			ConnectorTimeOut = 60000,		// 连接后无操作的超时时间 单位 : 毫秒

			ConncectTime = 3000,			// 重连定时器
			SerializeBuffLength = 1024 * 1024 * 20 + 24,	// 序列化消息的buff长度

			CUT_MSGCHILDBEGIN = 65535,		// 分割的头消息
			CUT_MSGCHILD = 65534,		// 分割的子消息

			///////////////////////////////////////////////////////////////////////////////////
			///////////////////////////////////////////////////////////////////////////////////
			ConnectEvent = 1,	// 连接事件
			DisconnectEvent = 2,		// 关闭事件
			FailedEvent = 3,	// 失败事件
			ErrorEvent = 4,		// 错误事件
			ShutEvent = 5,		// 销毁时间
		};
	}

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 连接的数据
	class KFTrustee
	{
	public:
		KFTrustee()
		{
			_net_connector = nullptr;
			_time_out = 0;
			_is_trustee = true;
		}

		// 连接器
		KFNetConnector* _net_connector;

		// 连接时间
		uint64 _time_out;

		// 已经不再托管
		bool _is_trustee;
	};
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 连接参数
	class KFConnectData
	{
	public:
		KFConnectData() : _argument( nullptr ), _status( 0 )
		{
			_services = nullptr;
		}

		void* _argument;
		uint32 _status;
		void* _services;
	};
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 网络事件
	class KFEventData
	{
	public:
		KFEventData()
		{
			_id = 0;
			_type = 0;
			_code = 0;
			_data = nullptr;
		}

		// 类型
		uint32 _type;

		// id 
		uint32 _id;

		// 数据
		void* _data;

		// 错误码
		int32 _code;

		// 描述
		std::string _describe;
	};
}

#endif