#include "KFMongoReadExecute.hpp"

namespace KFrame
{
    KFResult< uint64 >::UniqueType KFMongoReadExecute::Count( const std::string& table )
    {
        Poco::MongoDB::Database db( _database );
        auto request = db.createQueryRequest( "$cmd" );
        request->setNumberToReturn( 1 );
        request->selector().add( MongoKeyword::_count, table );

        __NEW_RESULT__( uint64 );
        ResponseMessage response;
        auto ok = SendRequest( *request, response );
        if ( ok && response.documents().size() > 0 )
        {
            Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
            try
            {
                result->_value = doc->getInteger( "n" );
            }
            catch ( Poco::Exception& )
            {
                __LOG_DEBUG__( "table=[{}] count failed", table );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }

    KFResult< uint64 >::UniqueType KFMongoReadExecute::Count( const std::string& table, const std::string& field, uint64 key )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        auto& selector = request.selector();
        selector.add( field, key );

        auto& fields = request.returnFieldSelector();
        fields.add( MongoKeyword::_id, MongoKeyword::_asc );

        __NEW_RESULT__( uint64 );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            result->_value = response.documents().size();
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFResult< std::list< KFDBValue > >::UniqueType KFMongoReadExecute::QueryListRecord( const std::string& table, const KFMongoSelector& kfselector )
    {
        auto fullname = __FORMAT__( "{}.{}", _database, table );
        QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

        // 查询条件
        AddPocoDocument( request.selector(), kfselector );

        // 返回的字段
        if ( !kfselector._returns.empty() )
        {
            auto& fields = request.returnFieldSelector();
            auto iter = kfselector._returns.find( MongoKeyword::_id );
            if ( iter == kfselector._returns.end() )
            {
                fields.add( MongoKeyword::_id, 0 );
            }

            for ( auto& iter : kfselector._returns )
            {
                fields.add( iter.first, iter.second );
            }
        }

        // 数量限制
        if ( kfselector._limit_count != 0u )
        {
            request.setNumberToReturn( kfselector._limit_count );
        }
        ///////////////////////////////////////////////////////////////////////////////////
        __NEW_RESULT__( std::list< KFDBValue > );
        ResponseMessage response;
        auto ok = SendRequest( request, response );
        if ( ok )
        {
            for ( auto i = 0u; i < response.documents().size(); ++i )
            {
                KFDBValue dbvalue;
                Poco::MongoDB::Document::Ptr doc = response.documents()[ i ];
                try
                {
                    auto elements = doc->getSet();
                    for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                    {
                        auto& name = ( *iter )->name();
                        if ( kfselector._limit_returns.find( name ) != kfselector._limit_returns.end() )
                        {
                            continue;
                        }

                        auto type = ( *iter )->type();
                        if ( type == ElementTraits<Poco::Int64>::TypeId )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<Poco::Int64>* >( ( *iter ).get() );
                            if ( concrete != nullptr )
                            {
                                dbvalue.AddValue( name, concrete->value() );
                            }
                        }
                        else if ( type == ElementTraits<std::string>::TypeId )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                            if ( concrete != nullptr )
                            {
                                dbvalue.AddValue( name, concrete->value(), false );
                            }
                        }
                        else if ( type == ElementTraits<Poco::MongoDB::Binary::Ptr>::TypeId )
                        {
                            auto concrete = dynamic_cast< const ConcreteElement<Poco::MongoDB::Binary::Ptr>* >( ( *iter ).get() );
                            if ( concrete != nullptr )
                            {
                                dbvalue.AddValue( name, concrete->value()->toRawString(), true );
                            }
                        }
                    }
                }
                catch ( Poco::Exception& ex )
                {
                    __LOG_ERROR__( "mongo error=[{}]", ex.displayText() );
                }

                result->_value.emplace_back( dbvalue );
            }
        }
        else
        {
            result->SetCode( KFEnum::Error );
        }

        return result;
    }
}
