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

        // 逻辑
        void Run();

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

        // 返回结果列表
        std::vector< KFBaseResultQueue* > _result_queue_list;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFWriteExecute : public KFRedisExecute
    {
    public:
        KFWriteExecute();
        virtual ~KFWriteExecute();

        // 写操作
        virtual KFResult< voidptr >* VoidExecute( const std::string& strsql );
        virtual KFResult< uint64 >* UpdateExecute( const std::string& strsql );

        // 批量写
        virtual KFResult< voidptr >* Pipeline( const ListString& commands );

        /////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 返回结果
        KFResultQueue< voidptr > _void_result_queue;
        KFResultQueue< uint64 > _uint64_result_queue;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    class KFReadExecute : public KFRedisExecute
    {
    public:
        KFReadExecute();
        virtual ~KFReadExecute();

        ///////////////////////////////////////////////////////
        // 读操作
        virtual KFResult< uint64 >* UInt64Execute( const std::string& strsql );
        virtual KFResult< std::string >* StringExecute( const std::string& strsql );
        virtual KFResult< MapString >* MapExecute( const std::string& strsql );
        virtual KFResult< ListString >* ListExecute( const std::string& strsql );
        virtual KFResult< VectorString >* VectorExecute( const std::string& strsql );

        // 添加执行命令
        virtual KFResult< ListString >* ListPipelineExecute( const ListString& commands );
        virtual KFResult< std::list< MapString > >* ListMapPipelineExecute( const ListString& commands );
        ///////////////////////////////////////////////////////
    private:
        // 返回结果
        KFResultQueue< uint32 > _uint32_result_queue;
        KFResultQueue< std::string > _string_result_queue;
        KFResultQueue< uint64 > _uint64_result_queue;
        KFResultQueue< MapString > _map_result_queue;
        KFResultQueue< VectorString > _vector_result_queue;
        KFResultQueue< ListString > _list_result_queue;
        KFResultQueue< std::list< MapString > > _list_map_result_queue;
    };
}
#endif