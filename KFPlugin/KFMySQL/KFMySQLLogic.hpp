#ifndef __KF_MYSQL_LOGIC_H__
#define __KF_MYSQL_LOGIC_H__

#include "KFMySQLInterface.h"
#include "KFMySQLExecute.hpp"

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
        void Initialize( const std::string& name, const KFMySQLConnectOption* kfmysqloption );
        /////////////////////////////////////////////////////////////////////////////////////////////

        // 插入记录
        virtual bool Insert( const std::string& table, const StringMap& invalue );

        // 删除
        virtual bool Delete( const std::string& table );
        virtual bool Delete( const std::string& table, const std::string& key );
        virtual bool Delete( const std::string& table, const StringMap& keyvalues );

        // 更新多个字段
        virtual bool Update( const std::string& table, const StringMap& invalue );
        virtual bool Update( const std::string& table, const std::string& key, const StringMap& invalue );
        virtual bool Update( const std::string& table, const StringMap& keyvalue, const StringMap& invalue );

        // 查询所有
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table );
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table, const StringList& fields );
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table, const std::string& key );
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table, const std::string& key, const StringList& fields );
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table, const StringMap& key );
        virtual KFResult< StringMapList >::UniqueType Select( const std::string& table, const StringMap& key, const StringList& fields );

        // 事务( 目前没有加mysql事务功能 )
        virtual void Pipeline( StringList& commands );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        virtual KFResult< voidptr >::UniqueType VoidExecute( std::string& strsql );
        virtual KFResult< uint32 >::UniqueType UInt32Execute( std::string& strsql );
        virtual KFResult< uint64 >::UniqueType UInt64Execute( std::string& strsql );
        virtual KFResult< std::string >::UniqueType StringExecute( std::string& strsql );
        virtual KFResult< StringMap >::UniqueType MapExecute( std::string& strsql );
        virtual KFResult< StringList >::UniqueType ListExecute( std::string& strsql );
        virtual KFResult< StringMapList >::UniqueType ListMapExecute( std::string& strsql );

    private:
        // 读执行器
        KFMySQLReadExecute* _read_execute = nullptr;

        // 写执行器
        KFMySQLWriteExecute* _write_execute = nullptr;
    };
}
#endif