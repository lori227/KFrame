#ifndef __KF_MONGO_READ_EXECUTE_H__
#define __KF_MONGO_READ_EXECUTE_H__

#include "KFMongoExecute.hpp"

namespace KFrame
{
    class KFMongoReadExecute : public KFMongoExecute
    {
    public:
        KFMongoReadExecute() = default;
        virtual ~KFMongoReadExecute() = default;

        // 查询数量
        KFResult<uint64>::UniqueType Count( const std::string& table );
        KFResult<uint64>::UniqueType Count( const std::string& table, const std::string& field, uint64 key );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        template< typename ReturnType, typename KeyType, typename ValueType >
        typename KFResult<ReturnType>::UniqueType QueryValue( const std::string& table, const KeyType& key, const std::string& field )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

            // 查询条件
            request.selector().add( MongoKeyword::_id, key );

            // 查询字段
            request.returnFieldSelector().add( field, 1 );

            // 数量
            request.setNumberToReturn( 1 );

            typename __NEW_RESULT__( ReturnType );
            ResponseMessage response;
            auto ok = SendRequest( request, response );
            if ( ok )
            {
                if ( response.documents().size() > 0 )
                {
                    Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                    try
                    {
                        result->_value = doc->get<ValueType>( field );
                    }
                    catch ( Poco::Exception& )
                    {
                    }
                }
            }
            else
            {
                result->SetCode( KFEnum::Error );
            }

            return result;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        template< typename KeyType >
        typename KFResult<std::string>::UniqueType QueryBinary( const std::string& table, const KeyType& key, const std::string& field )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

            // 查询条件
            request.selector().add( MongoKeyword::_id, key );

            // 查询字段
            request.returnFieldSelector().add( field, 1 );

            // 数量
            request.setNumberToReturn( 1 );

            typename __NEW_RESULT__( std::string );
            ResponseMessage response;
            auto ok = SendRequest( request, response );
            if ( ok )
            {
                if ( response.documents().size() > 0 )
                {
                    Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                    try
                    {
                        auto value = doc->get<Poco::MongoDB::Binary::Ptr>( field );
                        if ( !value.isNull() )
                        {
                            result->_value = value->toRawString();
                        }
                    }
                    catch ( Poco::Exception& )
                    {
                    }
                }
            }
            else
            {
                result->SetCode( KFEnum::Error );
            }

            return result;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值列表
        template< class ReturnType, class KeyType, class ValueType >
        typename KFResult< ReturnType >::UniqueType QueryList( const std::string& table, const KeyType& key, const std::string& field )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

            // 查询条件
            request.selector().add( MongoKeyword::_id, key );

            // 查询字段
            request.returnFieldSelector().add( field, 1 );

            typename __NEW_RESULT__( ReturnType );
            ResponseMessage response;
            auto ok = SendRequest( request, response );
            if ( ok )
            {
                if ( response.documents().size() > 0 )
                {
                    Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                    try
                    {
                        auto array_value = doc->get< Array::Ptr >( field );
                        if ( !array_value.isNull() )
                        {
                            auto elements = array_value->getSet();
                            for ( auto iter = elements->begin(); iter != elements->end(); ++iter )
                            {
                                auto concrete = dynamic_cast< const ConcreteElement<ValueType>* >( ( *iter ).get() );
                                result->_value.push_back( concrete->value() );
                            }
                        }
                    }
                    catch ( Poco::Exception& )
                    {
                        __LOG_DEBUG__( "table=[{}] key=[{}] field=[{}] not exist", table, key, field );
                    }
                }
            }
            else
            {
                result->SetCode( KFEnum::Error );
            }

            return result;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询集合
        template< class KeyType >
        typename KFResult<KFDBValue>::UniqueType QueryRecord( const std::string& table, const KeyType& key, const StringList& field_list )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            QueryRequest request( fullname, QueryRequest::QUERY_DEFAULT );

            // 查询条件
            request.selector().add( MongoKeyword::_id, key );

            if ( !field_list.empty() )
            {
                auto& return_field_list = request.returnFieldSelector();
                for ( auto& field : field_list )
                {
                    return_field_list.add( field, 1 );
                }
            }

            __NEW_RESULT__( KFDBValue );
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
                            if ( name == MongoKeyword::_expire )
                            {
                                continue;
                            }

                            auto type = ( *iter )->type();
                            if ( type == ElementTraits<Poco::Int64>::TypeId )
                            {
                                auto concrete = dynamic_cast< const ConcreteElement<Poco::Int64>* >( ( *iter ).get() );
                                if ( concrete != nullptr )
                                {
                                    result->_value.AddValue( name, concrete->value() );
                                }
                            }
                            else if ( type == ElementTraits<std::string>::TypeId )
                            {
                                auto concrete = dynamic_cast< const ConcreteElement<std::string>* >( ( *iter ).get() );
                                if ( concrete != nullptr )
                                {
                                    result->_value.AddValue( name, concrete->value(), false );
                                }
                            }
                            else if ( type == ElementTraits<Poco::MongoDB::Binary::Ptr>::TypeId )
                            {
                                auto concrete = dynamic_cast< const ConcreteElement<Poco::MongoDB::Binary::Ptr>* >( ( *iter ).get() );
                                if ( concrete != nullptr )
                                {
                                    result->_value.AddValue( name, concrete->value()->toRawString(), true );
                                }
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
                result->SetCode( KFEnum::Error );
            }

            return result;
        }

        // 查询结合
        KFResult<std::list<KFDBValue>>::UniqueType QueryListRecord( const std::string& table, const KFMongoSelector& selector_data );
    };
}

#endif
