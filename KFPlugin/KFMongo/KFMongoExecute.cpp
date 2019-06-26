#include "KFMongoExecute.hpp"

namespace KFrame
{
    void KFMongoExecute::InitMongo( const KFMongoSetting* kfsetting )
    {
        _database = kfsetting->_database;
        return KFMongo::InitMongo( kfsetting );
    }

    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    bool KFWriteExecute::Update( const std::string& table, const MapString& keys, const MapString& values, uint32 updatetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, ( UpdateRequest::Flags )updatetype );

        // key
        if ( !keys.empty() )
        {
            auto& selector = request.selector();
            for ( auto& iter : keys )
            {
                selector.add( iter.first, iter.second );
            }
        }

        // values
        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        for ( auto& iter : values )
        {
            update->add( iter.first, iter.second );
        }
        request.update().add( _mongo_set, update );

        return SendRequest( request );
    }

    bool KFWriteExecute::Delete( const std::string& table, const MapString& keys, uint32 deletetype )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        DeleteRequest request( fullname, ( UpdateRequest::Flags )deletetype );

        if ( !keys.empty() )
        {
            auto& selector = request.selector();
            for ( auto& iter : keys )
            {
                selector.add( iter.first, iter.second );
            }
        }

        return SendRequest( request );
    }

    bool KFWriteExecute::Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

        auto index = 0u;
        Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
        for ( auto& value : inlist )
        {
            array->add( __TO_STRING__( index++ ), value );
        }

        Poco::MongoDB::Document::Ptr each = new Poco::MongoDB::Document();
        each->add( _mongo_each, array );

        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        update->add( field, each );

        request.update().add( _mongo_push, update );
        return SendRequest( request );
    }

    bool KFWriteExecute::Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        UpdateRequest request( fullname, UpdateRequest::UPDATE_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

        auto index = 0u;
        Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
        for ( auto& value : inlist )
        {
            array->add( __TO_STRING__( index++ ), value );
        }

        Poco::MongoDB::Document::Ptr in = new Poco::MongoDB::Document();
        in->add( _mongo_in, array );

        Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        update->add( field, in );

        request.update().add( _mongo_pull, update );
        return SendRequest( request );

        //for ( auto& value : inlist )
        //{
        //    request.update().clear();

        //    Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
        //    update->add( field, value );
        //    request.update().add( _mongo_pull, update );
        //    auto ok = SendRequest( request );
        //    if ( !ok )
        //    {
        //        return false;
        //    }
        //}

        //return true;
    }
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////
    KFResult< std::string >::UniqueType KFReadExecute::QueryString( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

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

    KFResult< uint64 >::UniqueType KFReadExecute::QueryUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

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
                    auto strvalue = doc->get<std::string>( field );
                    kfresult->_value = KFUtility::ToValue< uint64 >( strvalue );
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

    KFResult< std::list< uint64 > >::UniqueType KFReadExecute::QueryListUInt64( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

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
                        auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                        auto value = KFUtility::ToValue< uint64 >( concrete->value() );
                        kfresult->_value.push_back( value );
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

    KFResult< ListString >::UniqueType KFReadExecute::QueryListString( const std::string& table, const std::string& key, const std::string& field )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

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
                        auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
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

    KFResult< MapString >::UniqueType KFReadExecute::QueryMap( const std::string& table, const std::string& key, const ListString& fields )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( _mongo_id, key );

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
                        if ( name != _mongo_id && ElementTraits<std::string>::TypeId == ( *iter )->type() )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                            kfresult->_value[ name ] = concrete->value();
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
}
