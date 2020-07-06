#include "KFMongoWriteExecute.hpp"

namespace KFrame
{
    void KFMongoWriteExecute::AddExpireTime( Document& pocodocument, uint64 value, const KFDBValue& dbvalue )
    {
        auto nowtime = dbvalue.FindValue( __STRING__( time ) );
        if ( nowtime == 0u )
        {
            nowtime = KFGlobal::Instance()->_real_time;
        }

        Poco::Timestamp timestmp( ( nowtime + value ) * 1000000 );
        pocodocument.add( MongoKeyword::_expire, timestmp );
    }

    void KFMongoWriteExecute::AddDBValue( Document& pocodocument, const KFDBValue& dbvalue )
    {
        for ( auto& iter : dbvalue._int_list )
        {
            auto& name = iter.first;
            if ( name == MongoKeyword::_expire )
            {
                AddExpireTime( pocodocument, iter.second, dbvalue );
            }
            else
            {
                pocodocument.add( name, iter.second );
            }
        }

        for ( auto& iter : dbvalue._str_list )
        {
            pocodocument.add( iter.first, iter.second );
        }

        for ( auto& iter : dbvalue._bin_list )
        {
            Poco::MongoDB::Binary::Ptr binary = new Poco::MongoDB::Binary( iter.second );
            pocodocument.add( iter.first, binary );
        }
    }

    bool KFMongoWriteExecute::Insert( const std::string& table, const KFDBValue& dbvalue )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        InsertRequest request( fullname, InsertRequest::INSERT_DEFAULT );

        // values
        AddDBValue( request.addNewDocument(), dbvalue );

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Update( const std::string& table, const KFDBValue& dbvalue )
    {
        static KFMongoSelector kfseletor;
        return Update( table, dbvalue, kfseletor );
    }

    bool KFMongoWriteExecute::Update( const std::string& table, const KFDBValue& dbvalue, const KFMongoSelector& kfseletor )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_MULTIUPDATE );

        // seletor
        AddPocoDocument( request.selector(), kfseletor );

        // values
        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        AddDBValue( *update, dbvalue );
        request.update().add( MongoKeyword::_set, update );

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table, const KFMongoSelector& kfselector )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );

        AddPocoDocument( request.selector(), kfselector );

        return SendRequest( request );
    }
}
