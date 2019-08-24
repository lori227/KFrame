#ifndef __KF_MONGO_INTERFACE_H__
#define __KF_MONGO_INTERFACE_H__

#include "KFrame.h"
#include "KFMongoSelector.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoDriver
    {
    public:
        virtual ~KFMongoDriver() = default;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // database
        // 创建索引
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, bool unique = false, uint32 ttl = 0u ) = 0;
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, const MapString& values, bool unique = false, uint32 ttl = 0u ) = 0;

        // 设置过期时间
        // 设置有效时间
        virtual bool Expire( const std::string& table, uint64 key, uint64 validtime ) = 0;
        virtual bool Expire( const std::string& table, const std::string& key, uint64 validtime ) = 0;

        // 具体时间点
        virtual bool ExpireAt( const std::string& table, uint64 key, uint64 expiretime ) = 0;
        virtual bool ExpireAt( const std::string& table, const std::string& key, uint64 expiretime ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数值( 存在更新, 不存在插入 )
        template < class Key, class Value >
        bool InsertT( const std::string& table, Key key, const std::string& field, Value value )
        {
            auto strkey = __TO_STRING__( key );
            auto strvalue = __TO_STRING__( value );
            return Insert( table, strkey, field, strvalue );
        }

        virtual bool Insert( const std::string& table, const MapString& values ) = 0;
        virtual bool Insert( const std::string& table, uint64 key, const MapString& values ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const MapString& values ) = 0;
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 更新所有数值( 存在才更新 )
        template < class Value >
        bool UpdateT( const std::string& table, const std::string& field, Value value )
        {
            auto strvalue = __TO_STRING__( value );
            return Update( table, field, strvalue );
        }

        template < class Key, class Value >
        bool UpdateT( const std::string& table, const std::string& keyname, Key key, const std::string& field, Value value )
        {
            auto strkey = __TO_STRING__( key );
            auto strvalue = __TO_STRING__( value );
            return Update( table, keyname, strkey, field, strvalue );
        }

        virtual bool Update( const std::string& table, const MapString& values ) = 0;
        virtual bool Update( const std::string& table, const std::string& field, const std::string& value ) = 0;
        virtual bool Update( const std::string& table, const std::string& keyname, const std::string& key, const std::string& field, const std::string& value ) = 0;
        virtual bool Update( const std::string& table, const std::string& keyname, const std::string& key, const MapString& values ) = 0;

        // 更新单个数值( 存在才更新 )
        template < class Key, class Value >
        bool UpdateT( const std::string& table, Key key, const std::string& field, Value value )
        {
            auto strkey = __TO_STRING__( key );
            auto strvalue = __TO_STRING__( value );
            return Update( table, strkey, field, strvalue );
        }

        virtual bool Update( const std::string& table, uint64 key, const MapString& values ) = 0;
        virtual bool Update( const std::string& table, const std::string& key, const MapString& values ) = 0;
        virtual bool Update( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Update( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        template< class Key, class Value >
        bool PushT( const std::string& table, Key key, const std::string& field, Value value )
        {
            auto strkey = __TO_STRING__( key );
            auto strvalue = __TO_STRING__( value );
            return Push( table, strkey, field, strvalue );
        }

        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist ) = 0;
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, ListString& inlist ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除素组元素
        template< class Key, class Value >
        bool PullT( const std::string& table, Key key, const std::string& field, Value value )
        {
            auto strkey = __TO_STRING__( key );
            auto strvalue = __TO_STRING__( value );
            return Pull( table, strkey, field, strvalue );
        }

        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist ) = 0;
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, ListString& inlist ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数据
        virtual bool Delete( const std::string& table ) = 0;
        virtual bool Delete( const std::string& table, uint64 key ) = 0;
        virtual bool Delete( const std::string& table, const std::string& key ) = 0;
        virtual bool Delete( const std::string& table, const std::string& keyname, uint64 key ) = 0;
        virtual bool Delete( const std::string& table, const std::string& keyname, const std::string& key ) = 0;
        virtual bool Delete( const std::string& table, const KFMongoSelector& kfseletor ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field ) = 0;

        // 查询字串
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field ) = 0;

        // 查询数据
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field ) = 0;
        virtual KFResult< ListString >::UniqueType QueryListString( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< ListString >::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field ) = 0;

        // 查询集合
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, uint64 key ) = 0;
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, const std::string& key ) = 0;
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, uint64 key, const ListString& fields ) = 0;
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, const std::string& key, const ListString& fields ) = 0;

        // 查询列表集合
        virtual KFResult< ListMapString >::UniqueType QueryListMapString( const std::string& table, const KFMongoSelector& kfseletor ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoInterface : public KFModule
    {
    public:
        // 创建Execute
        virtual KFMongoDriver* Create( const std::string& module, uint32 logicid = 0 ) = 0;

    };

    ///////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_mongo, KFMongoInterface );
    ///////////////////////////////////////////////////////////////////////////////////////////////
}

#endif