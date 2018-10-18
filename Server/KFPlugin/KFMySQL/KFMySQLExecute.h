#ifndef __KF_MYSQL_EXECUTE_H__
#define __KF_MYSQL_EXECUTE_H__

#include "KFMySQL.h"
#include "Poco/SharedPtr.h"
#include "Poco/Data/Statement.h"
#include "Poco/Data/Extraction.h"
#include "Poco/Data/AbstractBinding.h"
#include "KFMySQLInterface.h"

namespace KFrame
{
    using namespace Poco::Data;
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMySQLExecute : public KFMySQL, public KFMySQLDriver
    {
    public:
        KFMySQLExecute();
        virtual ~KFMySQLExecute();

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
        virtual KFResult< std::list< MapString > >* Select( const std::string& table );
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const ListString& fields );
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const std::string& key );
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const std::string& key, const ListString& fields );
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const MapString& key );
        virtual KFResult< std::list< MapString > >* Select( const std::string& table, const MapString& key, const ListString& fields );


        // 事务( 目前没有加mysql事务功能 )
        virtual void Pipeline( const ListString& commands );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 逻辑
        void Run();
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

        virtual KFResult< voidptr >* VoidExecute( const std::string& strsql );
        virtual KFResult< uint32 >* UInt32Execute( const std::string& strsql );
        virtual KFResult< uint64 >* UInt64Execute( const std::string& strsql );
        virtual KFResult< std::string >* StringExecute( const std::string& strsql );
        virtual KFResult< MapString >* MapExecute( const std::string& strsql );
        virtual KFResult< ListString >* ListExecute( const std::string& strsql );
        virtual KFResult< std::list< MapString > >* ListMapExecute( const std::string& strsql );

    private:
        KFResultQueue< voidptr > _void_result_queue;
        KFResultQueue< uint32 > _uint32_result_queue;
        KFResultQueue< uint64 > _uint64_result_queue;
        KFResultQueue< std::string > _string_result_queue;
        KFResultQueue< MapString > _map_result_queue;
        KFResultQueue< ListString > _list_result_queue;
        KFResultQueue< std::list< MapString > > _list_map_result_queue;

        std::vector< KFBaseResultQueue* > _result_queue_list;

    };
}

#endif