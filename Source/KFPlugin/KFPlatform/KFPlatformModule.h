#ifndef __KF_PLATFORM_MODULE_H__
#define __KF_PLATFORM_MODULE_H__

/************************************************************************
//    @Module			:    平台模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-2-7
************************************************************************/

#include "KFrame.h"
#include "KFPlatformInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"


namespace KFrame
{
	class KFPlatformModule : public KFPlatformInterface
	{
	public:
		KFPlatformModule();
		~KFPlatformModule();
		
		// 加载配置
		virtual void InitModule();

		// 逻辑
		virtual void BeforeRun();

		// 关闭
		virtual void BeforeShut();

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
	protected:

		// 登录token
		std::string HandleLoginToken( const std::string& ip, const std::string& data );
		
		// 查询创建账号
		MapString QueryCreateAccount( const std::string& account, uint32 channel );
		
		// 激活账号
		std::string HandleActivationAccount( const std::string& ip, const std::string& data );
		
		// 验证token
		std::string HandleVerifyToken( const std::string& ip, const std::string& data );

		// 大厅信息
		std::string HandleUpdateZone( const std::string& ip, const std::string& data );

		// zone http server
		std::string HandleZoneHttp( const std::string& ip, const std::string& data );

		// 大厅丢失
		std::string HandleLostZone( const std::string& ip, const std::string& data );

		// 更新大厅状态
		std::string HandleUpdateZoneStatus( const std::string& ip, const std::string& data );

		// 查询服务器列表
		std::string HandleQueryZoneList( const std::string& ip, const std::string& data );

		// 查询状态
		std::string HandleQueryZoneStatus( const std::string& ip, const std::string& data );

		// 更新在线信息
		std::string HandleUpdateOnline( const std::string& ip, const std::string& data );

	private:
		void Run();

		// 保存账号token
		std::string SaveLoginToken( uint32 accountid, MapString& accountdata );

		// 保存额外数据
		void UpdateExtendData( uint32 accountid, uint32 channel, KFJson& kfjson );
	};
}



#endif