#ifndef __KF_REDIS_H__
#define __KF_REDIS_H__

#include "KFrame.h"

struct redisContext;
struct redisReply;

namespace KFrame
{

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    class KFHiRedis
    {
    public:
        KFHiRedis();
        ~KFHiRedis();

        // 连接数据库
        int32 Connect( const char* ip, int32 port, const char* password );
        int32 ReConnect();

        // 是否断开连接
        bool IsDisconnected();

        // 关闭连接
        void ShutDown();

        // 执行指令
        template< class T >
        bool Execute( T& value, const char* command );

        // 执行指令
        bool PipelineExecute( const ListString& commands );

        bool PipelineExecute( const ListString& commands, MapString& values );

        bool PipelineExecute( const ListString& commands, VectorString& values );
    protected:
        // redis执行
        redisReply* Execute( int32 type, const char* command );

    protected:
        // ip
        std::string _ip;

        // 端口
        int32 _port;

        // 密码
        std::string _password;

        // redis环境
        redisContext* _redis_context;
    };
}
#endif
