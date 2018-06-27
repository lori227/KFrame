#ifndef __KF_MYSQL_MODULE_H__
#define __KF_MYSQL_MODULE_H__

/************************************************************************
//    @Module			:    mysql数据库
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-10
************************************************************************/

#include "KFrame.h"
#include "KFMySQLInterface.h"
#include "KFMySQLExecute.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
	class KFMySQLModule : public KFMySQLInterface
	{
	public:
		KFMySQLModule();
		~KFMySQLModule();
		
		// 加载配置
		virtual void InitModule();

		// 关闭
		virtual void BeforeShut();
		virtual void ShutDown();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		// 创建Execute
		virtual KFMySQLDriver* CreateExecute( uint32 id );
		virtual KFMySQLDriver* CreateExecute( const std::string& filed, uint32 logicid = 0 );
		virtual KFMySQLDriver* CreateExecute( uint32 id, const std::string& user, const std::string& password, const std::string& database, const std::string& ip, uint32 port );

	protected:
		// 查询
		KFMySQLDriver* FindMySQLExecute( uint32 id );

		// 插入
		void InsertMySQLExecute( uint32 id, KFMySQLExecute* kfexecute );

	private:
		// 数据库列表
		typedef std::pair< uint32, uint32 > MySQLKey;
		KFMap< MySQLKey, const MySQLKey&, KFMySQLExecute, KFMutex > _mysql_execute_list;
	};
}



#endif