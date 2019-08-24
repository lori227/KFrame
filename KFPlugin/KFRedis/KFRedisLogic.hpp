#ifndef __KF_REDIS_LOGIC_H__
#define __KF_REDIS_LOGIC_H__

#include "KFRedisInterface.h"
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
        // 添加执行命令
        virtual void AppendCommand( const std::string& strsql );
        /////////////////////////////////////////////////////////////////////////////////////////////

        // 写操作
        virtual KFResult< voidptr >::UniqueType VoidExecute( const std::string& strsql );
        virtual KFResult< uint64 >::UniqueType UpdateExecute( const std::string& strsql );

        // 批量写
        virtual KFResult< voidptr >::UniqueType Pipeline();

        /////////////////////////////////////////////////////////////////////////////////////////////
        // 读操作
        // 查询数据库
        virtual KFResult< uint64 >::UniqueType UInt64Execute( const std::string& strsql );
        virtual KFResult< std::string >::UniqueType StringExecute( const std::string& strsql );
        virtual KFResult< MapString >::UniqueType MapExecute( const std::string& strsql );
        virtual KFResult< ListString >::UniqueType ListExecute( const std::string& strsql );
        virtual KFResult< VectorString >::UniqueType VectorExecute( const std::string& strsql );

        // 添加执行命令
        virtual KFResult< ListString >::UniqueType ListPipelineExecute();
        virtual KFResult< ListMapString >::UniqueType ListMapPipelineExecute();

    private:
        // 需要执行的命令集
        ListString _commands;

        // 读执行器
        KFReadExecute* _read_execute{ nullptr };

        // 写执行器
        KFWriteExecute* _write_execute{ nullptr };
    };
}
#endif