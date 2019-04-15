#ifndef __KF_HTTP_SERVER_H__
#define __KF_HTTP_SERVER_H__

#include "KFrame.h"

namespace Poco
{
    namespace Net
    {
        class HTTPServer;
    }
}

namespace KFrame
{
    class KFHttpFuncton
    {
    public:
        // http函数
        KFHttpMethodFunction _function;

        // 是否同步( 在主逻辑线程执行 )
        bool _sync;
    };

    class KFFunctionData
    {
    public:
        // 执行函数
        KFHttpFuncton* _kf_function;

        // ip
        std::string _ip;

        // 数据
        std::string _data;
    };

    class KFHttpServer
    {
    public:

        KFHttpServer();
        ~KFHttpServer();

        ////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////
        // 开启
        void Start( const std::string& ip, uint32 port, uint32 maxthread, uint32 maxqueue, uint32 idletime, bool keeplive );

        // 逻辑
        void Run();

        // 关闭
        void ShutDown();
        ///////////////////////////////////////////////////////////////////
        // 注册创建httphandle
        void RegisterMethonFunction( const std::string& url, bool sync, KFHttpMethodFunction& function );

        // 卸载
        void UnRegisterMethonFunction( const std::string& url );

        // 处理http请求
        std::string ProcessHttpRequest( const std::string& url, const std::string& ip, const std::string& data );

    private:
        Poco::Net::HTTPServer* _http_server;

        // 函数列表
        KFHashMap< std::string, const std::string&, KFHttpFuncton > _functions;

        // 线程锁
        KFMutex _kf_mutex;

        // 请求数据
        std::list< KFFunctionData* > _kf_function_data;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////

}


#endif