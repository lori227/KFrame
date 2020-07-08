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
        void Initialize( KFRedisType* kfredistype );

    protected:
        // 写操作
        virtual KFResult< voidptr >::UniqueType WriteVoid( const std::string& strsql );
        virtual KFResult< uint64 >::UniqueType WriteUInt64( const std::string& strsql );
        virtual KFResult< std::string >::UniqueType WriteString( const std::string& strsql );

        // 读操作
        virtual KFResult< uint64 >::UniqueType ReadUInt64( const std::string& strsql );
        virtual KFResult< std::string >::UniqueType ReadString( const std::string& strsql );
        virtual KFResult< StringMap >::UniqueType ReadMap( const std::string& strsql );
        virtual KFResult< StringPair >::UniqueType ReadPair( const std::string& strsql );
        virtual KFResult< StringList >::UniqueType ReadList( const std::string& strsql );
        virtual KFResult< StringVector >::UniqueType ReadVector( const std::string& strsql );
        virtual KFResult< StringMapList >::UniqueType ReadMapList( const std::string& strsql );
        virtual KFResult< StringPairList >::UniqueType ReadPairList( const std::string& strsql );

    private:

        // 读执行器
        KFRedisReadExecute* _read_execute{ nullptr };

        // 写执行器
        KFRedisWriteExecute* _write_execute{ nullptr };
    };
}
#endif