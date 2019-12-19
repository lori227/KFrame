#ifndef __KF_MONGO_LOGIC_H__
#define __KF_MONGO_LOGIC_H__

#include "KFMongoInterface.h"
#include "KFMongoReadExecute.hpp"
#include "KFMongoWriteExecute.hpp"

namespace KFrame
{
    // 读写分离,
    // 也支持单一节点, 配置表设置同一个数据库
    class KFMongoLogic : public KFMongoDriver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFMongoLogic();
        virtual ~KFMongoLogic();

        void ShutDown();
        void Initialize( KFMongoType* kfmongotype );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 创建索引
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, bool unique = false, uint32 ttl = 0u );
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, const MongoIndexType& values, bool unique = false, uint32 ttl = 0u );

        // 设置过期时间
        virtual bool Expire( const std::string& table, uint64 key, uint64 validtime );
        virtual bool Expire( const std::string& table, const std::string& key, uint64 validtime );

        // 具体时间点
        virtual bool ExpireAt( const std::string& table, uint64 key, uint64 expiretime );
        virtual bool ExpireAt( const std::string& table, const std::string& key, uint64 expiretime );

        // 查询数量
        virtual KFResult< uint64 >::UniqueType Count( const std::string& table );
        virtual KFResult< uint64 >::UniqueType Count( const std::string& table, const std::string& field, uint64 key );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数值( 存在更新, 不存在插入 )
        virtual bool Insert( const std::string& table, const KFDBValue& dbvalue );

        virtual bool Insert( const std::string& table, uint64 key, const KFDBValue& dbvalue );
        virtual bool Insert( const std::string& table, const std::string& key, const KFDBValue& dbvalue );

        virtual bool Insert( const std::string& table, const KFDBValue& dbvalue, const KFMongoSelector& kfseletor );
        virtual bool Insert( const std::string& table, const std::string& field, uint64 value, const KFMongoSelector& kfseletor );
        virtual bool Insert( const std::string& table, const std::string& field, const std::string& value, const KFMongoSelector& kfseletor );

        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数据
        virtual bool Delete( const std::string& table );
        virtual bool Delete( const std::string& table, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& key );
        virtual bool Delete( const std::string& table, const std::string& keyname, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& keyname, const std::string& key );
        virtual bool Delete( const std::string& table, const KFMongoSelector& kfseletor );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::list<uint64>& values );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::list<uint64>& values );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const StringList& values );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const StringList& values );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数组元素
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, std::list<uint64>& values );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, std::list<uint64>& values );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, StringList& values );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, StringList& values );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< std::list< std::string > >::UniqueType QueryListString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< std::list< std::string > >::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field );

        // 查询集合
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, uint64 key );
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, const std::string& key );
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, uint64 key, const StringList& fields );
        virtual KFResult< KFDBValue >::UniqueType QueryRecord( const std::string& table, const std::string& key, const StringList& fields );
        virtual KFResult< std::list< KFDBValue > >::UniqueType QueryListRecord( const std::string& table, const KFMongoSelector& kfseletor );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 读执行器
        KFMongoReadExecute* _read_execute = nullptr;

        // 写执行器
        KFMongoWriteExecute* _write_execute = nullptr;
    };
}
#endif