#ifndef __KF_MYSQL_MODULE_H__
#define __KF_MYSQL_MODULE_H__

/************************************************************************
//    @Module			:    mysql数据库
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-10
************************************************************************/

#include "KFMySQLInterface.h"
#include "KFMySQLLogic.hpp"
#include "KFMySQLConfig.hpp"

namespace KFrame
{
    class KFMySQLModule : public KFMySQLInterface
    {
    public:
        KFMySQLModule() = default;
        ~KFMySQLModule() = default;

        // 运行
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 创建Execute
        virtual std::shared_ptr<KFMySQLDriver> Create( const std::string& module, uint32 logic_id = 0 );

    protected:
        // 查找mysql配置
        const KFMySQLConnectOption* FindMySQLConnectOption( const std::string& module, uint32 logic_id );

        // 查询
        std::shared_ptr<KFMySQLLogic> FindMySQLLogic( uint32 logic_id );

        // 插入
        void InsertMySQLLogic( uint32 id, std::shared_ptr<KFMySQLLogic> mysql_logic );

    private:
        // 互斥量
        KFMutex _kf_mutex;

        // 数据库列表
        typedef std::pair<ThreadId, uint32> MySQLKey;
        KFMap<MySQLKey, KFMySQLLogic> _mysql_logic_map;
    };
}



#endif