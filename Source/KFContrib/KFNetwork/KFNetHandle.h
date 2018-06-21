#ifndef __NET_HANDLE_H__
#define __NET_HANDLE_H__

#include "KFNetConnector.h"

namespace KFrame
{
	class KFNetServerEngine;
	class KFNetServerServices;
	class KFNetHandle : public KFNetConnector
	{
	public:
		KFNetHandle();
		virtual ~KFNetHandle();
		
		// 初始化
		void InitHandle( uint32 id, void* uvtcp, KFNetServerServices* netservices );
	
		// 关闭服务
		void CloseHandle();

		// 关闭连接
		virtual void CloseSession();

		// 断开事件
		virtual void OnDisconnect( const char* error, int32 code );

		// 设置id
		void SetID( uint32 id );
		
		// 是否是服务器
		bool IsServer() const;
		
		// 设置托管超时时间
		void SetTrusteeTimeout( uint64 time );

		// 判断是否超时
		bool IsTrusteeTimeout( uint64 nowtime );

		//////////////////////////////////////////////////////////////////////////////////////////////
	protected:
		// 关闭回调
		static void OnShutCallBack( uv_handle_t* handle );
	
	public:
		// appid
		uint32 _app_id;
		
		// 类型
		std::string _app_type;

		// 名字
		std::string _app_name;

		// ip
		std::string _listen_ip;

		// port
		uint32 _listen_port;

		// 远程ip
		std::string _remote_ip;

	protected:
		// 网络服务
		uv_tcp_t * _uv_tcp;

		// 托管结束时间
		uint64 _trustee_timeout;
	};
}


#endif