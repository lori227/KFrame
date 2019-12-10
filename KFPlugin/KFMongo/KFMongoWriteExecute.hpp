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
        bool ExpireAt( const std::string& table, const KeyType& key, uint64 expiretime )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( MongoKeyword::_id, key );

            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            Poco::Timestamp timestmp( expiretime * 1000000 );
            update->add( MongoKeyword::_expire, timestmp );
            request.update().add( MongoKeyword::_set, update );
            return SendRequest( request );
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数据
        bool Insert( const std::string& table, const KFDBValue& dbvalue );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新数据
        bool Update( const std::string& table, const KFDBValue& dbvalue, const KFMongoSelector& kfseletor );

        template< class KeyType >
        bool Update( const std::string& table, const KeyType& key, const KFDBValue& dbvalue )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( MongoKeyword::_id, key );

            // values
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            AddDBValue( *update, dbvalue );
            request.update().add( MongoKeyword::_set, update );

            return SendRequest( request );
        }

        template< class KeyType, class ValueType >
        bool Update( const std::string& table, const std::string& keyname, const KeyType& keyvalue, const std::string& field, const ValueType& value )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // key
            request.selector().add( keyname, keyvalue );

            // value
            Poco::MongoDB::Document::Ptr update = new Poco::MongoDB::Document();
            update->add( field, value );
            request.update().add( MongoKeyword::_set, update );

            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数据
        bool Delete( const std::string& table );
        bool Delete( const std::string& table, const KFMongoSelector& kfselector );

        template< class ValueType >
        bool Delete( const std::string& table, const std::string& keyname, const ValueType& keyvalue )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            DeleteRequest request( fullname, DeleteRequest::DELETE_DEFAULT );

            request.selector().add( keyname, keyvalue );
            return SendRequest( request );
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        template< class KeyType, class ValueType >
        bool Push( const std::string& table, const std::string& keyname, const KeyType& keyvalue, const std::string& field, const std::list<ValueType>& values )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_UPSERT );

            // selector
            request.selector().add( keyname, keyvalue );

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
        bool Pull( const std::string& table, const std::string& keyname, const KeyType& keyvalue, const std::string& field, const std::list<ValueType>& values )
        {
            auto fullname = __FORMAT__( "{}.{}", _database, table );
            UpdateRequest request( fullname, UpdateRequest::UPDATE_DEFAULT );

            // selector
            request.selector().add( keyname, keyvalue );

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
        void AddExpireTime( Document& pocodocument, uint64 value, const KFDBValue& dbvalue );

        // 添加数据
        void AddDBValue( Document& pocodocument, const KFDBValue& dbvalue );
    };
}

#endif