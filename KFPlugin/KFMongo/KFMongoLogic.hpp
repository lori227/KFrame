#ifndef __KF_MONGO_LOGIC_H__
#define __KF_MONGO_LOGIC_H__

#include "KFMongoInterface.h"
#include "KFMongoExecute.hpp"

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
        virtual bool CreateIndex( const std::string& table, const std::string& indexname, const MapString& values, bool unique = false, uint32 ttl = 0u );

        // 设置过期时间
        virtual bool Expire( const std::string& table, uint64 key, uint64 validtime );
        virtual bool Expire( const std::string& table, const std::string& key, uint64 validtime );

        // 具体时间点
        virtual bool ExpireAt( const std::string& table, uint64 key, uint64 expiretime );
        virtual bool ExpireAt( const std::string& table, const std::string& key, uint64 expiretime );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 插入数据( 存在更新, 不存在插入 )
        virtual bool Insert( const std::string& table, const MapString& values );
        virtual bool Insert( const std::string& table, uint64 key, const MapString& values );
        virtual bool Insert( const std::string& table, const std::string& key, const MapString& values );
        virtual bool Insert( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Insert( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        /////////////////////////////////////////////////////////////////////////////////////////////

        // 更新数据( 存在才更新 )
        virtual bool Update( const std::string& table, const MapString& values );
        virtual bool Update( const std::string& table, const std::string& field, const std::string& value );
        virtual bool Update( const std::string& table, const std::string& keyname, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Update( const std::string& table, const std::string& keyname, const std::string& key, const MapString& values );

        // 更新数据( 存在才更新 )
        virtual bool Update( const std::string& table, uint64 key, const MapString& values );
        virtual bool Update( const std::string& table, const std::string& key, const MapString& values );
        virtual bool Update( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Update( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 添加数组元素
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist );
        virtual bool Push( const std::string& table, uint64 key, const std::string& field, ListString& inlist );
        virtual bool Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist );
        /////////////////////////////////////////////////////////////////////////////////////////////
        // 删除数组元素
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, const std::string& value );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, std::list< uint32 >& inlist );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, std::list< uint32 >& inlist );
        virtual bool Pull( const std::string& table, uint64 key, const std::string& field, ListString& inlist );
        virtual bool Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist );
        /////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool Delete( const std::string& table );
        virtual bool Delete( const std::string& table, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& key );
        virtual bool Delete( const std::string& table, const std::string& keyname, uint64 key );
        virtual bool Delete( const std::string& table, const std::string& keyname, const std::string& key );
        virtual bool Delete( const std::string& table, const KFMongoSelector& kfseletor );
        /////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< std::string >::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< ListString >::UniqueType QueryListString( const std::string& table, uint64 key, const std::string& field );
        virtual KFResult< ListString >::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field );
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, uint64 key );
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, const std::string& key );
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, uint64 key, const ListString& fields );
        virtual KFResult< MapString >::UniqueType QueryMap( const std::string& table, const std::string& key, const ListString& fields );
        /////////////////////////////////////////////////////////////////////////////////////////////
        virtual KFResult< ListMapString >::UniqueType QueryListMapString( const std::string& table, const KFMongoSelector& kfseletor );
    private:
        // 读执行器
        KFReadExecute* _read_execute = nullptr;

        // 写执行器
        KFWriteExecute* _write_execute = nullptr;
    };
}
#endif