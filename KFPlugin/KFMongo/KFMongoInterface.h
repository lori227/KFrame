#ifndef __KF_MONGO_INTERFACE_H__
#define __KF_MONGO_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFMongoDriver
    {
    public:
        virtual ~KFMongoDriver() = default;

        // 插入数据
        virtual bool Insert( const std::string& table, uint64 key, const MapString& invalue ) = 0;
        virtual bool Insert( const std::string& table, const std::string& key, const MapString& invalue ) = 0;
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