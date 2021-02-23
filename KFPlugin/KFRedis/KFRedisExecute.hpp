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
        KFRedisExecute() = default;
        virtual ~KFRedisExecute();

        // 初始化
        int32 Initialize( const std::string& name, const std::string& ip, uint32 port, const std::string& password );

        // 关闭
        void ShutDown();

    protected:

        // 连接
        int32 TryConnect();

        // 验证密码
        uint32 TryAuthPassword( const std::string& password );

        // 是否断开连接
        bool IsDisconnected();

        // 执行语句
        redisReply* Execute( const std::string& sql );
        redisReply* TryExecute( const std::string& sql );

    protected:
        // 名字
        std::string _name;

        // ip
        std::string _ip;

        // 端口
        uint32 _port = 0;

        // 密码
        std::string _password;

        // redis环境
        redisContext* _redis_context = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFRedisWriteExecute : public KFRedisExecute
    {
    public:
        KFRedisWriteExecute() = default;
        virtual ~KFRedisWriteExecute() = default;

        KFResult<voidptr>::UniqueType WriteVoid( const std::string& sql );
        KFResult<uint64>::UniqueType WriteUInt64( const std::string& sql );
        KFResult<std::string>::UniqueType WriteString( const std::string& sql );
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
        KFResult<uint64>::UniqueType ReadUInt64( const std::string& sql );
        KFResult<std::string>::UniqueType ReadString( const std::string& sql );
        KFResult<StringMap>::UniqueType ReadMap( const std::string& sql );
        KFResult<StringPair>::UniqueType ReadPair( const std::string& sql );
        KFResult<StringList>::UniqueType ReadList( const std::string& sql );
        KFResult<StringVector>::UniqueType ReadVector( const std::string& sql );
        KFResult<StringMapList>::UniqueType ReadMapList( const std::string& sql );
        KFResult<StringPairList>::UniqueType ReadPairList( const std::string& sql );

    };
}
#endif