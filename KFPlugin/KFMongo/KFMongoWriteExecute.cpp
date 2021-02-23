#include "KFMongoWriteExecute.hpp"

namespace KFrame
{
    void KFMongoWriteExecute::AddExpireTime( Document& poco_document, uint64 value, const KFDBValue& db_value )
    {
        auto now_time = db_value.FindValue( __STRING__( time ) );
        if ( now_time == 0u )
        {
            now_time = KFGlobal::Instance()->_real_time;
        }

        Poco::Timestamp timestamp( ( now_time + value ) * 1000000 );
        poco_document.add( MongoKeyword::_expire, timestamp );
    }

    void KFMongoWriteExecute::AddDBValue( Document& poco_document, const KFDBValue& db_value )
    {
        for ( auto& iter : db_value._int_list )
        {
            auto& name = iter.first;
            if ( name == MongoKeyword::_expire )
            {
                AddExpireTime( poco_document, iter.second, db_value );
            }
            else
            {
                poco_document.add( name, iter.second );
            }
        }

        for ( auto& iter : db_value._str_list )
        {
            poco_document.add( iter.first, iter.second );
        }

        for ( auto& iter : db_value._bin_list )
        {
            Poco::MongoDB::Binary::Ptr binary = new Poco::MongoDB::Binary( iter.second );
            poco_document.add( iter.first, binary );
        }
    }

    bool KFMongoWriteExecute::Insert( const std::string& table, const KFDBValue& db_value )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        InsertRequest request( fullname, InsertRequest::INSERT_DEFAULT );

        // values
        AddDBValue( request.addNewDocument(), db_value );

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Update( const std::string& table, const KFDBValue& db_value )
    {
        static KFMongoSelector selector_data;
        return Update( table, db_value, selector_data );
    }

    bool KFMongoWriteExecute::Update( const std::string& table, const KFDBValue& db_value, const KFMongoSelector& selector_data )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_MULTIUPDATE );

        // seletor
        AddPocoDocument( request.selector(), selector_data );

        // values
        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        AddDBValue( *update, db_value );
        request.update().add( MongoKeyword::_set, update );

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table, const KFMongoSelector& selector_data )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );

        AddPocoDocument( request.selector(), selector_data );

        return SendRequest( request );
    }
}
