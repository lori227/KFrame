#ifndef __KF_HTTP_HANDLE_H__
#define __KF_HTTP_HANDLE_H__

#include "KFrame.h"
#include "Poco/Net/HTTPRequestHandler.h"

namespace KFrame
{
    class KFHttpServer;
    class KFHttpHandle : public Poco::Net::HTTPRequestHandler
    {
    public:
        KFHttpHandle( KFHttpServer* kfhttpserver );

        // 处理请求
        virtual void handleRequest( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response );

    private:
        // httpserver
        KFHttpServer* _http_server;
    };
}


#endif
