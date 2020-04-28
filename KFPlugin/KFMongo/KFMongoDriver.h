#ifndef __KF_MONGO_DRIVER_H__
#define __KF_MONGO_DRIVER_H__

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
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, const MongoIndexType& values, bool unique = false, uint32 ttl = 0u ) = 0;

        // 设置过期时间
        virtual bool Expire( const std::string& table, uint64 key, uint64 validtime ) = 0;
        virtual bool Expire( const std::string& table, const std::string& key, uint64 validtime ) = 0;

        // 具体时间点
        virtual bool ExpireAt( const std::string& table, uint64 key, uint64 expiretime ) = 0;
        virtual bool ExpireAt( const std::string& table, const std::string& key, uint64 expiretime ) = 0;

        // 查询纪录数量
        virtual KFResult< uint64 >::UniqueType Count( const std::string& table ) = 0;
        virtual KFResult< uint64 >::UniqueType Count( const std::string& table, const std::string& field, uint64 key ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数值( 存在更新, 不存在插入 )
        virtual bool Insert( const std::string& table, const KFDBValue& dbvalue ) = 0;
        virtual bool Insert( const std::string& table, uint64 key, const KFDBValue& dbvalue ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const KFDBValue& dbvalue ) = 0;
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, uint64 value ) = 0;
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value, bool isbinary = false ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, uint64 value ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value, bool isbinary = false ) = 0;
        virtual bool Insert( const std::string& table, const KFDBValue& dbvalue, const KFMongoSelector& kfseletor ) = 0;
        virtual bool Insert( const std::string& table, const std::string& field, uint64 value, const KFMongoSelector& kfseletor ) = 0;
        virtual bool Insert( const std::string& table, const std::string& field, const std::string& value, const KFMongoSelector& kfseletor ) = 0;

        // 操作数值
        virtual bool Operate( const std::string& table, const std::string& field, uint32 operate, uint64 value ) = 0;
        virtual bool Operate( const std::string& table, uint64 key, const std::string& field, uint32 operate, uint64 value ) = 0;
        virtual bool Operate( const std::string& table, const std::string& key, const std::string& field, uint32 operate, uint64 value ) = 0;
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
        // 添加数组元素
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, uint64 value ) = 0;
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, uint64 value ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const UInt64List& values ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const UInt64List& values ) = 0;
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const StringList& values ) = 0;
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const StringList& values ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数组元素
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, uint64 value ) = 0;
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, uint64 value ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value ) = 0;
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, UInt64List& values ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, UInt64List& values ) = 0;
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, StringList& values ) = 0;
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, StringList& values ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field ) = 0;
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field ) = 0;
        virtual KFResult< std::string >::UniqueType QueryBinary( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< std::string >::UniqueType QueryBinary( const std::string& table, const std::string& key, const std::string& field ) = 0;

        // 查询数组
        virtual KFResult< UInt64List >::UniqueType QueryListUInt64( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< UInt64List >::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field ) = 0;
        virtual KFResult< StringList >::UniqueType QueryListString( const std::string& table, uint64 key, const std::string& field ) = 0;
        virtual KFResult< StringList >::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field ) = 0;

        // 查询集合
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, uint64 key ) = 0;
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, const std::string& key ) = 0;
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, uint64 key, const StringList& fields ) = 0;
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, const std::string& key, const StringList& fields ) = 0;
        virtual KFResult< std::list< KFDBValue > >::UniqueType QueryListRecord( const std::string& table, const KFMongoSelector& kfseletor ) = 0;
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

#endif