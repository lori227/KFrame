#include "KFMongoExecute.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    void KFMongoExecute::InitMongo( const KFMongoSetting* kfsetting )
    {
        return KFMongo::InitMongo( kfsetting );
    }

    void KFMongoExecute::AddDocumentValue( Document& pocodocument, const std::string& key, const std::string& value )
    {
        static MapString values;
        AddDocumentValue( pocodocument, key, value, values );
    }

    void KFMongoExecute::AddDocumentValue( Document& pocodocument, const std::string& key, const std::string& value, const MapString& values )
    {
        if ( KFUtility::IsNumber( value ) )
        {
            auto intvalue = KFUtility::ToValue<int64>( value );
            if ( key == MongoKeyword::_expire && !values.empty() )
            {
                auto nowtime = KFGlobal::Instance()->_real_time;
                auto iter = values.find( __KF_STRING__( time ) );
                if ( iter != values.end() )
                {
                    nowtime = KFUtility::ToValue<uint64>( iter->second );
                }

                Poco::Timestamp timestmp( ( nowtime + intvalue ) * 1000000 );
                pocodocument.add( key, timestmp );
            }
            else
            {
                pocodocument.add( key, intvalue );
            }
        }
        else
        {
            pocodocument.add( key, value );
        }
    }

    bool KFMongoExecute::CreateIndex( const std::string& table, const std::string& indexname, const MapString& values, bool unique, uint32 ttl )
    {
        try
        {
            Poco::MongoDB::Document::Ptr temp = new Poco::MongoDB::Document();
            for ( auto& iter : values )
            {
                AddDocumentValue( ( *temp ), iter.first, iter.second );
            }

            Poco::MongoDB::Database db( _database );
            auto newindexname = __FORMAT__( "idx_{}", indexname );
            auto doc = db.ensureIndex( *_connection, table, newindexname, temp, unique, false, 0, ttl );
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

    void KFMongoExecute::AddPocoDocument( Document& pocodocument, const KFMongoSelector& kfselector )
    {
        if ( kfselector._key != 0u )
        {
            pocodocument.add( MongoKeyword::_id, ( int64 )kfselector._key );
        }

        if ( !kfselector._document._expressions.empty() || !kfselector._document._documents.empty() )
        {
            Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
            AddPocoDocument( *array, &kfselector._document );
            pocodocument.add( kfselector._document._condition, array );
        }
    }

    void KFMongoExecute::AddPocoDocument( Document& pocodocument, const KFMongoDocument* kfdocument )
    {
        for ( auto expression : kfdocument->_expressions )
        {
            Poco::MongoDB::Document::Ptr temp = new Poco::MongoDB::Document();
            if ( expression->_type == MongoKeyword::_eq )
            {
                AddDocumentValue( ( *temp ), expression->_name, expression->_value1 );
            }
            else if ( expression->_type == MongoKeyword::_in )
            {
                Poco::MongoDB::Array::Ptr valuearray = new Poco::MongoDB::Array();
                AddDocumentValue( ( *valuearray ), "0", expression->_value1 );
                AddDocumentValue( ( *valuearray ), "1", expression->_value2 );

                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                child->add( MongoKeyword::_in, valuearray );
                temp->add( expression->_name, child );
            }
            else
            {
                Poco::MongoDB::Document::Ptr child = new Poco::MongoDB::Document();
                AddDocumentValue( ( *child ), expression->_type, expression->_value1 );
                temp->add( expression->_name, child );
            }

            pocodocument.add( __TO_STRING__( pocodocument.size() ), temp );
        }

        for ( auto document : kfdocument->_documents )
        {
            AddPocoDocument( pocodocument, document );
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFMongoWriteExecute::ExpireAt( const std::string& table, const std::string& key, uint64 expiretime )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

        // key
        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        Poco::Timestamp timestmp( expiretime * 1000000 );
        update->add( MongoKeyword::_expire, timestmp );
        request.update().add( MongoKeyword::_set, update );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Insert( const std::string& table, const MapString& values, uint32 inserttype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        InsertRequest request( fullname, ( InsertRequest::Flags )inserttype );

        // values
        auto& document = request.addNewDocument();
        for ( auto& iter : values )
        {
            AddDocumentValue( document, iter.first, iter.second, values );
        }

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Update( const std::string& table, const MapString& keys, const MapString& values, uint32 updatetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, ( UpdateRequest::Flags )updatetype );

        // key
        if ( !keys.empty() )
        {
            auto& selector = request.selector();
            for ( auto& iter : keys )
            {
                AddDocumentValue( selector, iter.first, iter.second );
            }
        }

        // values
        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        for ( auto& iter : values )
        {
            AddDocumentValue( ( *update ), iter.first, iter.second, values );
        }
        request.update().add( MongoKeyword::_set, update );

        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table, uint32 deletetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, ( UpdateRequest::Flags )deletetype );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table, const std::string& keyname, const std::string& keyvalue, uint32 deletetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, ( UpdateRequest::Flags )deletetype );

        auto& selector = request.selector();
        AddDocumentValue( selector, keyname, keyvalue );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Delete( const std::string& table, const KFMongoSelector& kfselector, uint32 deletetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, ( UpdateRequest::Flags )deletetype );

        auto& selector = request.selector();
        AddPocoDocument( selector, kfselector );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto index = 0u;
        Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
        for ( auto& value : inlist )
        {
            AddDocumentValue( ( *array ), __TO_STRING__( index++ ), value );
        }

        Poco::MongoDB::Document::Ptr each = new Poco::MongoDB::Document();
        each->add( MongoKeyword::_each, array );

        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        update->add( field, each );

        request.update().add( MongoKeyword::_push, update );
        return SendRequest( request );
    }

    bool KFMongoWriteExecute::Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_DEFAULT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto index = 0u;
        Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
        for ( auto& value : inlist )
        {
            AddDocumentValue( ( *array ), __TO_STRING__( index++ ), value );
        }

        Poco::MongoDB::Document::Ptr in = new Poco::MongoDB::Document();
        in->add( MongoKeyword::_in, array );

        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        update->add( field, in );

        request.update().add( MongoKeyword::_pull, update );
        return SendRequest( request );
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    KFResult< std::string >::UniqueType KFMongoReadExecute::QueryString( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto& fields = request.returnFieldSelector();
        fields.add( field, 1 );

        __NEW_RESULT__( std::string );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            if ( response.documents().size() > 0 )
            {
                Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                try
                {
                    kfresult->_value = doc->get<std::string>( field );
                }
                catch ( Poco::Exception& )
                {
                    __LOG_DEBUG__( "table=[{}] key=[{}] field=[{}] not exist!", table, key, field );
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< uint64 >::UniqueType KFMongoReadExecute::QueryUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto& fields = request.returnFieldSelector();
        fields.add( field, 1 );

        __NEW_RESULT__( uint64 );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            if ( response.documents().size() > 0 )
            {
                Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                try
                {
                    kfresult->_value = doc->get<int64>( field );
                }
                catch ( Poco::Exception& )
                {
                    __LOG_DEBUG__( "table=[{}] key=[{}] field=[{}] failed!", table, key, field );
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< std::list< uint64 > >::UniqueType KFMongoReadExecute::QueryListUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto& fields = request.returnFieldSelector();
        fields.add( field, 1 );

        __NEW_RESULT__( std::list< uint64 > );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            if ( response.documents().size() > 0 )
            {
                Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                try
                {
                    auto elements = doc->get< Array::Ptr >( field )->getSet();
                    for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                    {
                        auto concrete = dynamic_cast< const ConcreteElement<int64>* >( ( *iter ).get() );
                        kfresult->_value.push_back( concrete->value() );
                    }
                }
                catch ( Poco::Exception& )
                {
                    __LOG_DEBUG__( "table=[{}] key=[{}] field=[{}] not exist!", table, key, field );
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< ListString >::UniqueType KFMongoReadExecute::QueryListString( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        AddDocumentValue( selector, MongoKeyword::_id, key );

        auto& fields = request.returnFieldSelector();
        fields.add( field, 1 );

        __NEW_RESULT__( ListString );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            if ( response.documents().size() > 0 )
            {
                Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                try
                {
                    auto elements = doc->get< Array::Ptr >( field )->getSet();
                    for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                    {
                        if ( ElementTraits<std::string>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                            kfresult->_value.push_back( concrete->value() );
                        }
                        else if ( ElementTraits<int64>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<int64>* >( ( *iter ).get() );
                            kfresult->_value.push_back( __TO_STRING__( concrete->value() ) );
                        }
                    }
                }
                catch ( Poco::Exception& )
                {
                    __LOG_DEBUG__( "table=[{}] key=[{}] field=[{}] not exist!", table, key, field );
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< MapString >::UniqueType KFMongoReadExecute::QueryMap( const std::string& table, const std::string& key, const ListString& fields )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( MongoKeyword::_id, key );

        if ( !fields.empty() )
        {
            auto& returnfields = request.returnFieldSelector();
            for ( auto& field : fields )
            {
                returnfields.add( field, 1 );
            }
        }

        __NEW_RESULT__( MapString );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            if ( response.documents().size() > 0 )
            {
                Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                try
                {
                    auto elements = doc->getSet();
                    for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                    {
                        auto& name = ( *iter )->name();
                        if ( name == MongoKeyword::_id || name == MongoKeyword::_expire )
                        {
                            continue;
                        }

                        if ( ElementTraits<std::string>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                            kfresult->_value[ name ] = concrete->value();
                        }
                        else if ( ElementTraits<int64>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<int64>* >( ( *iter ).get() );
                            kfresult->_value[ name ] = __TO_STRING__( concrete->value() );
                        }
                    }
                }
                catch ( Poco::Exception& )
                {
                }
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }

    KFResult< ListMapString >::UniqueType KFMongoReadExecute::QueryListMapString( const std::string& table, const KFMongoSelector& kfselector )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        AddPocoDocument( selector, kfselector );
        ///////////////////////////////////////////////////////////////////////////////////
        __NEW_RESULT__( ListMapString );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            for ( auto i = 0u; i < response.documents().size(); ++i )
            {
                MapString values;
                Poco::MongoDB::Document::Ptr doc = response.documents()[ i ];
                try
                {
                    auto elements = doc->getSet();
                    for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                    {
                        auto& name = ( *iter )->name();
                        if ( name == MongoKeyword::_id || name == MongoKeyword::_expire )
                        {
                            continue;
                        }

                        if ( ElementTraits<std::string>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                            values[ name ] = concrete->value();
                        }
                        else if ( ElementTraits<int64>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<int64>* >( ( *iter ).get() );
                            values[ name ] = __TO_STRING__( concrete->value() );
                        }
                    }
                }
                catch ( Poco::Exception& )
                {
                }

                kfresult->_value.push_back( values );
            }
        }
        else
        {
            kfresult->SetResult( KFEnum::Error );
        }

        return kfresult;
    }
}
