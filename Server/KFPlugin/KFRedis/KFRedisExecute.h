#ifndef __KF_REDIS_EXECUTE_H__
#define __KF_REDIS_EXECUTE_H__

#include "KFRedisInterface.h"
#include "hiredis/hiredis.h"

namespace KFrame
{
    class KFRedisExecute : public KFRedisDriver
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////
        KFRedisExecute();
        ~KFRedisExecute();

        // 初始化
        int32 Initialize( const std::string& ip, uint32 port, const std::string& password );

        // 关闭
        void ShutDown();

        // 切换索引
        void SelectIndex( uint32 index );


    protected:
        // 连接
        int32 TryConnect();

        // 是否断开连接
        bool IsDisconnected();
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////

        // 执行更新
        virtual KFResult< voidptr >* UpdateExecute( const char* function, uint32 line, const std::string& strsql );

        // 查询数据库
        virtual KFResult< uint32 >* UInt32Execute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< uint64 >* UInt64Execute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< std::string >* StringExecute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< MapString >* MapExecute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< GreaterMapString >* GreaterMapExecute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< ListString >* ListExecute( const char* function, uint32 line, const std::string& strsql );
        virtual KFResult< VectorString >* VectorExecute( const char* function, uint32 line, const std::string& strsql );

        // 添加执行命令
        virtual void AppendCommand( const std::string& strsql );
        virtual KFResult< voidptr >* Pipeline( const char* function, uint32 line );
        virtual KFResult< ListString >* ListPipelineExecute( const char* function, uint32 line );
        virtual KFResult< std::list< MapString > >* ListMapPipelineExecute( const char* function, uint32 line );

    private:
        // 执行语句
        redisReply* Execute( const std::string& strsql );
        redisReply* TryExecute( KFBaseResult* kfresult, const char* function, uint32 line, const std::string& strsql );

    private:
        // ip
        std::string _ip;

        // 端口
        uint32 _port;

        // 密码
        std::string _password;

        // redis环境
        redisContext* _redis_context;

        // 数据库索引
        uint32 _index;

        // 需要执行的命令集
        ListString _commands;

        //////////////////////////////////////////////////////////////
        // 返回结果
        KFResult< voidptr > _void_result;
        KFResult< uint32 > _uint32_result;
        KFResult< std::string > _string_result;
        KFResult< uint64 > _uint64_result;
        KFResult< MapString > _map_result;
        KFResult< GreaterMapString > _greater_map_result;
        KFResult< VectorString > _vector_result;
        KFResult< ListString > _list_result;
        KFResult< std::list< MapString > > _list_map_result;
    };
}
#endif