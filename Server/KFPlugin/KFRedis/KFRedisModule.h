#ifndef __KF_REDIS_MODULE_H__
#define __KF_REDIS_MODULE_H__

/************************************************************************
//    @Module			:    redis数据库
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-11
************************************************************************/

#include "KFrame.h"
#include "KFRedisInterface.h"

namespace KFrame
{
	class KFRedisDriver;
	class KFRedisModule : public KFRedisInterface
	{
	public:
		KFRedisModule();
		~KFRedisModule();
		
		// 加载配置
		virtual void InitModule();

		// 关闭
		virtual void BeforeShut();
		virtual void ShutDown();

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// 创建RedisExecute
		virtual KFRedisDriver* CreateExecute( uint32 id );
		virtual KFRedisDriver* CreateExecute( const std::string& field, uint32 logicid = 0 );
		virtual KFRedisDriver* CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password );
	};
}



#endif