#ifndef __KF_WRITE_MONGO_EXECUTE_H__
#define __KF_WRITE_MONGO_EXECUTE_H__

#include "KFMongoExecute.hpp"

namespace KFrame
{
    class KFMongoWriteExecute : public KFMongoExecute
    {
    public:
        KFMongoWriteExecute() = default;
        virtual ~KFMongoWriteExecute() = default;

        // 设置过期时间
        template< class KeyType >
        bool ExpireAt( const std::string& table, const KeyType& key, uint64 expire_time )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( MongoKeyword::_id, key );

            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            Poco::Timestamp timestamp( expire_time * 1000000 );
            update->add( MongoKeyword::_expire, timestamp );
            request.update().add( MongoKeyword::_set, update );
            return SendRequest( request );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数据
        bool Insert( const std::string& table, const KFDBValue& db_value );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新数据
        bool Update( const std::string& table, const KFDBValue& db_value );
        bool Update( const std::string& table, const KFDBValue& db_value, const KFMongoSelector& selector_data );

        template< class KeyType >
        bool Update( const std::string& table, const KeyType& key, const KFDBValue& db_value )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( MongoKeyword::_id, key );

            // values
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            AddDBValue( *update, db_value );
            request.update().add( MongoKeyword::_set, update );

            return SendRequest( request );
        }

        template< class KeyType, class ValueType >
        bool Update( const std::string& table, const std::string& key_name, const KeyType& key_value, const std::string& field, const ValueType& value )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( key_name, key_value );

            // value
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            update->add( field, value );
            request.update().add( MongoKeyword::_set, update );

            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template< class KeyType >
        uint64 Operate( const std::string& table, const std::string& key_name, const KeyType& key_value, const std::string& field, uint32 operate, uint64 value )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );
            if ( !key_name.empty() )
            {
                request.selector().add( key_name, key_value );
            }

            uint64 result = 0u;
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            switch ( operate )
            {
            case KFEnum::Set:
                update->add( field, value );
                request.update().add( MongoKeyword::_set, update );
                break;
            case KFEnum::Add:
                update->add( field, value );
                request.update().add( MongoKeyword::_inc, update );
                break;
            case KFEnum::Dec:
                update->add( field, ( 0 - value ) );
                request.update().add( MongoKeyword::_inc, update );
                break;
            default:
                break;
            }

            ResponseMessage response;
            auto ok = SendRequest( request, response );
            if ( ok )
            {
                if ( response.documents().size() > 0 )
                {
                    Poco::MongoDB::Document::Ptr doc = response.documents()[ 0 ];
                    try
                    {
                        result = doc->get<Poco::UInt64>( field );
                    }
                    catch ( Poco::Exception& )
                    {
                    }
                }
            }

            return result;
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数据
        bool Delete( const std::string& table );
        bool Delete( const std::string& table, const KFMongoSelector& selector_data );

        template< class ValueType >
        bool Delete( const std::string& table, const std::string& key_name, const ValueType& key_value )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );

            request.selector().add( key_name, key_value );
            return SendRequest( request );
        }

        // 删除字段
        bool DeleteField( const std::string& table, uint64 key, const std::string& field )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( MongoKeyword::_id, key );

            // value
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            update->add( field, _invalid_string );
            request.update().add( MongoKeyword::_unset, update );

            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        template< class KeyType, class ValueType >
        bool Push( const std::string& table, const std::string& key_name, const KeyType& key_value, const std::string& field, const std::list<ValueType>& values )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // selector
            request.selector().add( key_name, key_value );

            auto index = 0u;
            Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
            for ( auto& value : values )
            {
                array->add( __TO_STRING__( index++ ), value );
            }

            Poco::MongoDB::Document::Ptr each = new Poco::MongoDB::Document();
            each->add( MongoKeyword::_each, array );

            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            update->add( field, each );

            request.update().add( MongoKeyword::_push, update );
            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数组元素
        template< class KeyType, class ValueType >
        bool Pull( const std::string& table, const std::string& key_name, const KeyType& key_value, const std::string& field, const std::list<ValueType>& values )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_DEFAULT );

            // selector
            request.selector().add( key_name, key_value );

            auto index = 0u;
            Poco::MongoDB::Array::Ptr array = new Poco::MongoDB::Array();
            for ( auto& value : values )
            {
                array->add( __TO_STRING__( index++ ), value );
            }

            Poco::MongoDB::Document::Ptr in = new Poco::MongoDB::Document();
            in->add( MongoKeyword::_in, array );

            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            update->add( field, in );

            request.update().add( MongoKeyword::_pull, update );
            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    protected:
        // 添加有效时间
        void AddExpireTime( Document& poco_document, uint64 value, const KFDBValue& db_value );

        // 添加数据
        void AddDBValue( Document& poco_document, const KFDBValue& db_value );
    };
}

#endif