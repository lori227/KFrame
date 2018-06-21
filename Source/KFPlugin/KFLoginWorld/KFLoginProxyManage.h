#ifndef __KF_LOGIN_PROXY_MANAGE_H__
#define __KF_LOGIN_PROXY_MANAGE_H__

#include "KFrame.h"

namespace KFrame
{
	class KFLoginProxy
	{
	public:
		KFLoginProxy()
		{
			_client_count = 0;
			_proxy_id = 0;
			_port = 0;
		}

		// id
		uint32 _proxy_id;

		// ip
		std::string _ip;

		// 端口
		uint32 _port;

		// 当前连接的客户端
		uint32 _client_count;
	};

	// 集群管理器
	class KFLoginProxyManage : public KFSingleton< KFLoginProxyManage >
	{
	public:
		KFLoginProxyManage();
		~KFLoginProxyManage();
		
		// 添加登录节点
		KFLoginProxy* AddLoginProxy( uint32 handleid, const std::string& ip, uint32 port );

		// 删除登录节点
		void RemoveLoginProxy( uint32 handleid );

		// 更新登录节点
		void UpdateLoginProxy( uint32 handleid, uint32 clientcount, const std::string& ip, uint32 port );

		// 选择登录节点
		KFLoginProxy* SelectLoginProxy();

		// 查找登录节点
		KFLoginProxy* FindLoginProxy( uint32 handleid );

	public:
		// 登录节点列表
		KFMap< uint32, uint32, KFLoginProxy > _kf_proxy_list;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_login_proxy_manage = KFLoginProxyManage::Instance();
	///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif