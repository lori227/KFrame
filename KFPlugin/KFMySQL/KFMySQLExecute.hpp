#ifndef __KF_MYSQL_EXECUTE_H__
#define __KF_MYSQL_EXECUTE_H__

#include "KFMySQL.hpp"
#include "Poco/SharedPtr.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/AbstractBinding.h"

namespace KFrame
{
    using namespace Poco::Data;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMySQLExecute : public KFMySQL
    {
    public:
        KFMySQLExecute() = default;
        virtual ~KFMySQLExecute() = default;

    protected:
        // 转译字符
        void FormatSlashes( std::string& sql );

        // 执行操作
        bool ExecuteSql( std::string& sql );
        bool ExecuteSql( Statement& statement, std::string& sql );

        // 判断是否断线
        bool CheckDisconnected( int32 code );

        // 格式化
        std::string FormatKeyString( const StringMap& keys );
        std::string FormatFieldString( const StringList& field_list );
        std::string FormatUpdateString( const StringMap& values );
    };


    class KFMySQLWriteExecute : public KFMySQLExecute
    {
    public:
        KFMySQLWriteExecute() = default;
        virtual ~KFMySQLWriteExecute() = default;

        // 插入记录
        bool Insert( const std::string& table, const StringMap& values );

        // 删除
        bool Delete( const std::string& table );
        bool Delete( const std::string& table, const std::string& key );
        bool Delete( const std::string& table, const StringMap& keys );

        // 更新多个字段
        bool Update( const std::string& table, const StringMap& values );
        bool Update( const std::string& table, const std::string& key, const StringMap& values );
        bool Update( const std::string& table, const StringMap& keys, const StringMap& values );

        // 事务( 目前没有加mysql事务功能 )
        void Pipeline( StringList& command_list );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult<voidptr>::UniqueType VoidExecute( std::string& sql );
    };

    class KFMySQLReadExecute : public KFMySQLExecute
    {
    public:
        KFMySQLReadExecute() = default;
        virtual ~KFMySQLReadExecute() = default;

        // 查询所有
        KFResult<StringMapList>::UniqueType Select( const std::string& table );
        KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringList& field_list );
        KFResult<StringMapList>::UniqueType Select( const std::string& table, const std::string& key );
        KFResult<StringMapList>::UniqueType Select( const std::string& table, const std::string& key, const StringList& field_list );
        KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringMap& keys );
        KFResult<StringMapList>::UniqueType Select( const std::string& table, const StringMap& keys, const StringList& field_list );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult<uint32>::UniqueType UInt32Execute( std::string& sql );
        KFResult<uint64>::UniqueType UInt64Execute( std::string& sql );
        KFResult<std::string>::UniqueType StringExecute( std::string& sql );
        KFResult<StringMap>::UniqueType MapExecute( std::string& sql );
        KFResult<StringList>::UniqueType ListExecute( std::string& sql );
        KFResult<StringMapList>::UniqueType ListMapExecute( std::string& sql );
    };
}

#endif