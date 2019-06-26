#ifndef __KF_MONGO_EXECUTE_H__
#define __KF_MONGO_EXECUTE_H__

#include "KFMongo.hpp"

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
    protected:
        // 数据库名
        std::string _database;
    };


    class KFWriteExecute : public KFMongoExecute
    {
    public:
        KFWriteExecute() = default;
        virtual ~KFWriteExecute() = default;

        // 插入数据
        bool Insert( const std::string& table, const std::string& key, const MapString& invalue );

        // 更新数据
        bool Update( const std::string& table, const std::string& key, const MapString& invalue );
    };

    class KFReadExecute : public KFMongoExecute
    {
    public:
        KFReadExecute() = default;
        virtual ~KFReadExecute() = default;

    };
}

#endif