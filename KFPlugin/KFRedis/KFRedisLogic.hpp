#ifndef __KF_REDIS_LOGIC_H__
#define __KF_REDIS_LOGIC_H__

#include "KFRedisDriver.h"
#include "KFRedisExecute.hpp"
#include "KFRedisConfig.hpp"

namespace KFrame
{
    // 读写分离,
    // 也支持单一redis节点, 配置表设置同一个数据库
    class KFRedisLogic : public KFRedisDriver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFRedisLogic();
        virtual ~KFRedisLogic();

        // 关闭
        void ShutDown();

        // 初始化
        void Initialize( const std::string& name, const KFRedisConnectOption* redis_option );

    protected:
        // 写操作
        virtual KFResult<voidptr>::UniqueType WriteVoid( const std::string& sql );
        virtual KFResult<uint64>::UniqueType WriteUInt64( const std::string& sql );
        virtual KFResult<std::string>::UniqueType WriteString( const std::string& sql );

        // 读操作
        virtual KFResult<uint64>::UniqueType ReadUInt64( const std::string& sql );
        virtual KFResult<std::string>::UniqueType ReadString( const std::string& sql );
        virtual KFResult<StringMap>::UniqueType ReadMap( const std::string& sql );
        virtual KFResult<StringPair>::UniqueType ReadPair( const std::string& sql );
        virtual KFResult<StringList>::UniqueType ReadList( const std::string& sql );
        virtual KFResult<StringVector>::UniqueType ReadVector( const std::string& sql );
        virtual KFResult<StringMapList>::UniqueType ReadMapList( const std::string& sql );
        virtual KFResult<StringPairList>::UniqueType ReadPairList( const std::string& sql );

    private:

        // 读执行器
        KFRedisReadExecute* _read_execute = nullptr;

        // 写执行器
        KFRedisWriteExecute* _write_execute = nullptr;
    };
}
#endif