#ifndef __KF_MYSQL_LOGIC_H__
#define __KF_MYSQL_LOGIC_H__

#include "KFMySQLInterface.h"
#include "KFMySQLExecute.hpp"
#include "KFMySQLConfig.hpp"

namespace KFrame
{
    // 读写分离,
    // 也支持单一节点, 配置表设置同一个数据库
    class KFMySQLLogic : public KFMySQLDriver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFMySQLLogic();
        virtual ~KFMySQLLogic();

        // 关闭
        void ShutDown();

        // 初始化
        void Initialize( KFMySQLType* kfmysqltype );
        /////////////////////////////////////////////////////////////////////////////////////////////

        // 插入记录
        virtual bool Insert( const std::string& table, const MapString& invalue );

        // 删除
        virtual bool Delete( const std::string& table );
        virtual bool Delete( const std::string& table, const std::string& key );
        virtual bool Delete( const std::string& table, const MapString& keyvalues );

        // 更新多个字段
        virtual bool Update( const std::string& table, const MapString& invalue );
        virtual bool Update( const std::string& table, const std::string& key, const MapString& invalue );
        virtual bool Update( const std::string& table, const MapString& keyvalue, const MapString& invalue );

        // 查询所有
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table );
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const ListString& fields );
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const std::string& key );
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const std::string& key, const ListString& fields );
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const MapString& key );
        virtual KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const MapString& key, const ListString& fields );

        // 事务( 目前没有加mysql事务功能 )
        virtual void Pipeline( const ListString& commands );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        virtual KFResult< voidptr >::UniqueType VoidExecute( const std::string& strsql );
        virtual KFResult< uint32 >::UniqueType UInt32Execute( const std::string& strsql );
        virtual KFResult< uint64 >::UniqueType UInt64Execute( const std::string& strsql );
        virtual KFResult< std::string >::UniqueType StringExecute( const std::string& strsql );
        virtual KFResult< MapString >::UniqueType MapExecute( const std::string& strsql );
        virtual KFResult< ListString >::UniqueType ListExecute( const std::string& strsql );
        virtual KFResult< std::list< MapString > >::UniqueType ListMapExecute( const std::string& strsql );

    private:
        // 读执行器
        KFReadExecute* _read_execute = nullptr;

        // 写执行器
        KFWriteExecute* _write_execute = nullptr;
    };
}
#endif