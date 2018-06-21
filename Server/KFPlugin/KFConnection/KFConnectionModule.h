#ifndef __KF_CONNECTION_MODULE_H__
#define __KF_CONNECTION_MODULE_H__

/************************************************************************
//    @Moudle			:    游戏连接模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-1
************************************************************************/

#include "KFrame.h"
#include "KFConnectionInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFConnectionModule : public KFConnectionInterface
	{
	public:
		KFConnectionModule();
		~KFConnectionModule();

		// 初始化
		virtual void InitModule();
		
		// 关闭
		virtual void ShutDown();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// 监听端口
		virtual uint32 GetListenPort() const;

		// 平台访问地址
		virtual const std::string& FindPlatformAddress( uint32 id );

		// Master连接地址
		virtual const KFConnection* FindMasterConnection( const std::string& name );
	
		// 修改mastr连接id
		virtual void SetMasterConnection( const std::string& name, uint32 zoneid );
	};
}

#endif