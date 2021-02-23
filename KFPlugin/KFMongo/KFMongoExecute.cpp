#include "KFMongoExecute.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    void KFMongoExecute::InitMongo( const KFMongoConnectOption* connect_option, const KFMongoConnnectData* connect_data )
    {
        return KFMongo::InitMongo( connect_option, connect_data );
    }

    bool KFMongoExecute::IsIndexCreate( const std::string& table, const std::string& index_name )
    {
        auto iter = _table_index_list.find( table );
        if ( iter == _table_index_list.end() )
        {
            return false;
        }

        return iter->second.find( index_name ) != iter->second.end();
    }

    bool KFMongoExecute::CreateIndex( const std::string& table, const std::string& index_name, const MongoIndexType& values, bool unique, uint32 ttl )
    {
        if ( IsIndexCreate( table, index_name ) )
        {
            return true;
        }

        try
        {
            Poco::MongoDB::Document::Ptr temp = new Poco::MongoDB::Document();
            for ( auto& iter : values )
            {
                temp->add( iter.first, iter.second );
            }

            Poco::MongoDB::Database db( _database );
            auto new_index_name = __FORMAT__( "idx_{}", index_name );
            auto doc = db.ensureIndex( *_connection, table, new_index_name, temp, unique, false, 0, ttl );
            bool err = doc->get( "err" ).isNull();
            if ( !err )
            {
                return true;
            }
            __LOG_ERROR__( "mongo create index failed=[{}]", doc->toString() );
        }
        catch ( Poco::Exception& ex )
        {
            __LOG_ERROR__( "mongo create index error=[{}]", ex.displayText() );
        }

        return false;
    }

    void KFMongoExecute::AddPocoDocument( Document& poco_document, const KFMongoSelector& selector_data )
    {
        if ( selector_data._key != 0u )
        {
            poco_document.add( MongoKeyword::_id, selector_data._key );
        }

        auto mongo_document = &selector_data._document;
        if ( !mongo_document->_int_expressions.empty() ||
                !mongo_document->_str_expressions.empty() ||
                !mongo_document->_documents.empty() )
        {
            Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
            AddPocoDocument( *array, mongo_document );
            poco_document.add( mongo_document->_condition, array );
        }
    }

    void KFMongoExecute::AddPocoDocument( Document& poco_document, const KFMongoDocument* mongo_document )
    {
        for ( auto expression : mongo_document->_int_expressions )
        {
            Poco::MongoDB::Document::Ptr temp = new Poco::MongoDB::Document();
            AddDocumentValue( *temp, expression );
            poco_document.add( __TO_STRING__( poco_document.size() ), temp );
        }

        for ( auto expression : mongo_document->_str_expressions )
        {
            Poco::MongoDB::Document::Ptr temp = new Poco::MongoDB::Document();
            AddDocumentValue( *temp, expression );
            poco_document.add( __TO_STRING__( poco_document.size() ), temp );
        }

        for ( auto document : mongo_document->_documents )
        {
            AddPocoDocument( poco_document, document );
        }
    }
}
