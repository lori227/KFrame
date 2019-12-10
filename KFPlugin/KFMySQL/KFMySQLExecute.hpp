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
        std::string FormatKeyString( const MapString& keyvalue );
        std::string FormatFieldString( const ListString& fields );
        std::string FormatUpdateString( const MapString& updatevalue );
    };


    class KFMySQLWriteExecute : public KFMySQLExecute
    {
    public:
        KFMySQLWriteExecute() = default;
        virtual ~KFMySQLWriteExecute() = default;

        // 插入记录
        bool Insert( const std::string& table, const MapString& invalue );

        // 删除
        bool Delete( const std::string& table );
        bool Delete( const std::string& table, const std::string& key );
        bool Delete( const std::string& table, const MapString& keyvalues );

        // 更新多个字段
        bool Update( const std::string& table, const MapString& invalue );
        bool Update( const std::string& table, const std::string& key, const MapString& invalue );
        bool Update( const std::string& table, const MapString& keyvalue, const MapString& invalue );

        // 事务( 目前没有加mysql事务功能 )
        void Pipeline( ListString& commands );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult< voidptr >::UniqueType VoidExecute( std::string& strsql );
    };

    class KFMySQLReadExecute : public KFMySQLExecute
    {
    public:
        KFMySQLReadExecute() = default;
        virtual ~KFMySQLReadExecute() = default;

        // 查询所有
        KFResult< ListMapString >::UniqueType Select( const std::string& table );
        KFResult< ListMapString >::UniqueType Select( const std::string& table, const ListString& fields );
        KFResult< ListMapString >::UniqueType Select( const std::string& table, const std::string& key );
        KFResult< ListMapString >::UniqueType Select( const std::string& table, const std::string& key, const ListString& fields );
        KFResult< ListMapString >::UniqueType Select( const std::string& table, const MapString& key );
        KFResult< ListMapString >::UniqueType Select( const std::string& table, const MapString& key, const ListString& fields );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult< uint32 >::UniqueType UInt32Execute( std::string& strsql );
        KFResult< uint64 >::UniqueType UInt64Execute( std::string& strsql );
        KFResult< std::string >::UniqueType StringExecute( std::string& strsql );
        KFResult< MapString >::UniqueType MapExecute( std::string& strsql );
        KFResult< ListString >::UniqueType ListExecute( std::string& strsql );
        KFResult< ListMapString >::UniqueType ListMapExecute( std::string& strsql );
    };
}

#endif