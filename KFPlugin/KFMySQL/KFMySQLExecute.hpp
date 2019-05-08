#ifndef __KF_MYSQL_EXECUTE_H__
#define __KF_MYSQL_EXECUTE_H__

#include "KFMySQL.hpp"
#include "Poco/SharedPtr.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/AbstractBinding.h"
#include "KFMySQLInterface.h"

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
        char* FormatSlashes( char* buffer, uint32 length );

        // 执行操作
        bool ExecuteSql( const std::string& sql );
        bool ExecuteSql( Statement& statement );

        // 判断是否断线
        bool CheckDisconnected( int32 code );

        // 格式化
        std::string FormatKeyString( const MapString& keyvalue );
        std::string FormatFieldString( const ListString& fields );
        std::string FormatUpdateString( const MapString& updatevalue );
    };


    class KFWriteExecute : public KFMySQLExecute
    {
    public:
        KFWriteExecute() = default;
        virtual ~KFWriteExecute() = default;

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
        void Pipeline( const ListString& commands );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult< voidptr >::UniqueType VoidExecute( const std::string& strsql );
    };

    class KFReadExecute : public KFMySQLExecute
    {
    public:
        KFReadExecute() = default;
        virtual ~KFReadExecute() = default;

        // 查询所有
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table );
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const ListString& fields );
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const std::string& key );
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const std::string& key, const ListString& fields );
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const MapString& key );
        KFResult< std::list< MapString > >::UniqueType Select( const std::string& table, const MapString& key, const ListString& fields );

        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        KFResult< uint32 >::UniqueType UInt32Execute( const std::string& strsql );
        KFResult< uint64 >::UniqueType UInt64Execute( const std::string& strsql );
        KFResult< std::string >::UniqueType StringExecute( const std::string& strsql );
        KFResult< MapString >::UniqueType MapExecute( const std::string& strsql );
        KFResult< ListString >::UniqueType ListExecute( const std::string& strsql );
        KFResult< std::list< MapString > >::UniqueType ListMapExecute( const std::string& strsql );
    };
}

#endif