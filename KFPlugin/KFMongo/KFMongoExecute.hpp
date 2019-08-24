#ifndef __KF_MONGO_EXECUTE_H__
#define __KF_MONGO_EXECUTE_H__

#include "KFMongo.hpp"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include "Poco/MongoDB/DeleteRequest.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/MongoDB/Array.h"
#include "Poco/MongoDB/Database.h"
#include "KFMongoSelector.h"

namespace KFrame
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoExecute : public KFMongo
    {
    public:
        KFMongoExecute() = default;
        virtual ~KFMongoExecute() = default;

        // 初始化
        virtual void InitMongo( const KFMongoSetting* kfsetting );

        // 创建索引
        bool CreateIndex( const std::string& table, const std::string& indexname, const MapString& values, bool unique, uint32 ttl );

    protected:
        // 添加选择器
        void AddPocoDocument( Document& pocodocument, const KFMongoSelector& kfselector );
        void AddPocoDocument( Document& pocodocument, const KFMongoDocument* kfdocument );

        // 添加数值
        void AddDocumentValue( Document& pocodocument, const std::string& key, const std::string& value );
        void AddDocumentValue( Document& pocodocument, const std::string& key, const std::string& value, const MapString& values );
    protected:
        // 数据库名
        std::string _database;
    };

    class KFWriteExecute : public KFMongoExecute
    {
    public:
        KFWriteExecute() = default;
        virtual ~KFWriteExecute() = default;

        // 设置过期时间
        bool ExpireAt( const std::string& table, const std::string& key, uint64 expiretime );

        // 查询数据
        bool Insert( const std::string& table, const MapString& values, uint32 inserttype );

        // 更新数据
        bool Update( const std::string& table, const MapString& keys, const MapString& values, uint32 updatetype );

        // 删除数据
        bool Delete( const std::string& table, uint32 deletetype );
        bool Delete( const std::string& table, const KFMongoSelector& kfselector, uint32 deletetype );
        bool Delete( const std::string& table, const std::string& keyname, const std::string& keyvalue, uint32 deletetype );

        // 添加数组元素
        bool Push( const std::string& table, const std::string& key, const std::string& field, ListString& inlist );

        // 删除数组元素
        bool Pull( const std::string& table, const std::string& key, const std::string& field, ListString& inlist );
    };

    class KFReadExecute : public KFMongoExecute
    {
    public:
        KFReadExecute() = default;
        virtual ~KFReadExecute() = default;

        // 查询
        KFResult< uint64 >::UniqueType QueryUInt64( const std::string& table, const std::string& key, const std::string& field );
        KFResult< std::string >::UniqueType QueryString( const std::string& table, const std::string& key, const std::string& field );
        KFResult< std::list< uint64 > >::UniqueType QueryListUInt64( const std::string& table, const std::string& key, const std::string& field );
        KFResult< ListString >::UniqueType QueryListString( const std::string& table, const std::string& key, const std::string& field );
        KFResult< MapString >::UniqueType QueryMap( const std::string& table, const std::string& key, const ListString& fields );
        KFResult< ListMapString >::UniqueType QueryListMapString( const std::string& table, const KFMongoSelector& kfselector );
    };
}

#endif