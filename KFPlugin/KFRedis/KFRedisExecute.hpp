#ifndef __KF_REDIS_EXECUTE_H__
#define __KF_REDIS_EXECUTE_H__

#include "KFrame.h"
#include "hiredis/hiredis.h"

namespace KFrame
{
    class KFRedisExecute
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFRedisExecute();
        virtual ~KFRedisExecute();

        // 初始化
        int32 Initialize( const std::string& name, const std::string& ip, uint32 port, const std::string& password );

        // 关闭
        void ShutDown();

    protected:

        // 连接
        int32 TryConnect();

        // 是否断开连接
        bool IsDisconnected();

        // 执行语句
        redisReply* Execute( const std::string& strsql );
        redisReply* TryExecute( KFBaseResult* kfresult, const std::string& strsql );

    protected:
        // 名字
        std::string _name;

        // ip
        std::string _ip;

        // 端口
        uint32 _port;

        // 密码
        std::string _password;

        // redis环境
        redisContext* _redis_context;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFRedisWriteExecute : public KFRedisExecute
    {
    public:
        KFRedisWriteExecute() = default;
        virtual ~KFRedisWriteExecute() = default;

        // 写操作
        KFResult< voidptr >::UniqueType VoidExecute( const std::string& strsql );
        KFResult< uint64 >::UniqueType UpdateExecute( const std::string& strsql );

        // 批量写
        KFResult< voidptr >::UniqueType Pipeline( const StringList& commands );

        /////////////////////////////////////////////////////////////////////////////////////////////
    };
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFRedisReadExecute : public KFRedisExecute
    {
    public:
        KFRedisReadExecute() = default;
        virtual ~KFRedisReadExecute() = default;

        ///////////////////////////////////////////////////////
        // 读操作
        KFResult< uint64 >::UniqueType UInt64Execute( const std::string& strsql );
        KFResult< std::string >::UniqueType StringExecute( const std::string& strsql );
        KFResult< StringMap >::UniqueType MapExecute( const std::string& strsql );
        KFResult< StringList >::UniqueType ListExecute( const std::string& strsql );
        KFResult< StringVector >::UniqueType VectorExecute( const std::string& strsql );

        // 添加执行命令
        KFResult< StringList >::UniqueType ListPipelineExecute( const StringList& commands );
        KFResult< StringMapList >::UniqueType ListMapPipelineExecute( const StringList& commands );
        ///////////////////////////////////////////////////////
    };
}
#endif