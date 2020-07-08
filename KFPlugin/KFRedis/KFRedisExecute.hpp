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

        KFResult< voidptr >::UniqueType WriteVoid( const std::string& strsql );
        KFResult< uint64 >::UniqueType WriteUInt64( const std::string& strsql );
        KFResult< std::string >::UniqueType WriteString( const std::string& strsql );
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
        KFResult< uint64 >::UniqueType ReadUInt64( const std::string& strsql );
        KFResult< std::string >::UniqueType ReadString( const std::string& strsql );
        KFResult< StringMap >::UniqueType ReadMap( const std::string& strsql );
        KFResult< StringPair >::UniqueType ReadPair( const std::string& strsql );
        KFResult< StringList >::UniqueType ReadList( const std::string& strsql );
        KFResult< StringVector >::UniqueType ReadVector( const std::string& strsql );
        KFResult< StringMapList >::UniqueType ReadMapList( const std::string& strsql );
        KFResult< StringPairList >::UniqueType ReadPairList( const std::string& strsql );

    };
}
#endif