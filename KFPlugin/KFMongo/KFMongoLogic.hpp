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
        void Initialize( const std::string& name, const KFMongoConnectOption* mongo_option );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 创建索引
        virtual bool CreateIndex( const std::string& table, const std::string& index_name, bool unique = false, uint32 ttl = 0u );
        virtual bool CreateIndex( const std::string& table, const std::string& index_name, const MongoIndexType& values, bool unique = false, uint32 ttl = 0u );

        // 设置过期时间
        virtual bool Expire( const std::string& table, uint64 key, uint64 valid_time );
        virtual bool Expire( const std::string& table, const std::string& key, uint64 valid_time );

        // 具体时间点
        virtual bool ExpireAt( const std::string& table, uint64 key, uint64 expire_time );
        virtual bool ExpireAt( const std::string& table, const std::string& key, uint64 expire_time );

        // 查询数量
        virtual KFResult<uint64>::UniqueType Count( const std::string& table );
        virtual KFResult<uint64>::UniqueType Count( const std::string& table, const std::string& field, uint64 key );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数值( 存在更新, 不存在插入 )
        virtual bool Insert( const std::string& table, const KFDBValue& db_value );
        virtual bool Insert( const std::string& table, const std::string& field, uint64 value );
        virtual bool Insert( const std::string& table, const std::string& field, const std::string& value, bool is_binary = false );

        virtual bool Insert( const std::string& table, uint64 key, const KFDBValue& db_value );
        virtual bool Insert( const std::string& table, const std::string& key, const KFDBValue& db_value );

        virtual bool Insert( const std::string& table, const KFDBValue& db_value, const KFMongoSelector& selector_data );
        virtual bool Insert( const std::string& table, const std::string& field, uint64 value, const KFMongoSelector& selector_data );
        virtual bool Insert( const std::string& table, const std::string& field, const std::string& value, const KFMongoSelector& selector_data );

        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value, bool is_binary = false );
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value, bool is_binary = false );

        // 操作数值
        virtual uint64 Operate( const std::string& table, const std::string& field, uint32 operate, uint64 value );
        virtual uint64 Operate( const std::string& table, uint64 key, const std::string& field, uint32 operate, uint64 value );
        virtual uint64 Operate( const std::string& table, const std::string& key, const std::string& field, uint32 operate, uint64 value );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数据
        virtual bool Delete( const std::string& table );
        virtual bool Delete( const std::string& table, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& key );
        virtual bool Delete( const std::string& table, const std::string& key_name, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& key_name, const std::string& key );
        virtual bool Delete( const std::string& table, const KFMongoSelector& selector_data );

        // 删除字段
        virtual bool DeleteField( const std::string& table, uint64 key, const std::string& field );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const UInt64List& values );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const UInt64List& values );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const StringList& values );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const StringList& values );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数组元素
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, uint64 value );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, uint64 value );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, UInt64List& values );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, UInt64List& values );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, StringList& values );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, StringList& values );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 查询数值
        virtual KFResult<uint64>::UniqueType QueryUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult<uint64>::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult<std::string>::UniqueType QueryString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult<std::string>::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult<std::string>::UniqueType QueryBinary( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult<std::string>::UniqueType QueryBinary( const std::string& table, const std::string& key, const std::string& field );

        virtual KFResult<UInt64List>::UniqueType QueryListUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult<UInt64List>::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult<StringList>::UniqueType QueryListString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult<StringList>::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field );

        // 查询集合
        virtual KFResult<KFDBValue>::UniqueType QueryRecord( const std::string& table, uint64 key );
        virtual KFResult<KFDBValue>::UniqueType QueryRecord( const std::string& table, const std::string& key );
        virtual KFResult<KFDBValue>::UniqueType QueryRecord( const std::string& table, uint64 key, const StringList& field_list );
        virtual KFResult<KFDBValue>::UniqueType QueryRecord( const std::string& table, const std::string& key, const StringList& field_list );

        virtual KFResult<std::list<KFDBValue>>::UniqueType QueryListRecord( const std::string& table );
        virtual KFResult<std::list<KFDBValue>>::UniqueType QueryListRecord( const std::string& table, const KFMongoSelector& selector_data );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 读执行器
        KFMongoReadExecute* _read_execute = nullptr;

        // 写执行器
        KFMongoWriteExecute* _write_execute = nullptr;
    };
}
#endif