#ifndef __KF_MONGO_EXECUTE_H__
#define __KF_MONGO_EXECUTE_H__

#include "KFMongo.hpp"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/MongoDB/UpdateRequest.h"
#include "Poco/MongoDB/DeleteRequest.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/MongoDB/Array.h"

namespace KFrame
{
    static std::string _mongo_id = "_id";
    static std::string _mongo_set = "$set";
    static std::string _mongo_push = "$addToSet";
    static std::string _mongo_each = "$each";
    static std::string _mongo_pull = "$pull";
    static std::string _mongo_in = "$in";
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMongoExecute : public KFMongo
    {
    public:
        KFMongoExecute() = default;
        virtual ~KFMongoExecute() = default;

        // 初始化
        virtual void InitMongo( const KFMongoSetting* kfsetting );
    protected:
        // 数据库名
        std::string _database;
    };

    class KFWriteExecute : public KFMongoExecute
    {
    public:
        KFWriteExecute() = default;
        virtual ~KFWriteExecute() = default;

        // 更新数据
        bool Update( const std::string& table, const MapString& keys, const MapString& values, uint32 updatetype );

        // 删除数据
        bool Delete( const std::string& table, const MapString& keys, uint32 deletetype );

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
    };
}

#endif