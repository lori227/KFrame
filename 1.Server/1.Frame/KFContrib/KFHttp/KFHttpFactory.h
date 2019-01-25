#ifndef __KF_HTTP_FACTORY_H__
#define __KF_HTTP_FACTORY_H__

#include "KFDefine.h"
#include "KFrame.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPRequestHandler.h"

namespace KFrame
{
    class KFHttpServer;
    class KFHttpFactory : public Poco::Net::HTTPRequestHandlerFactory
    {
    public:
        KFHttpFactory( KFHttpServer* kfhttpserver );

        // 创建请求处理
        Poco::Net::HTTPRequestHandler* createRequestHandler( const Poco::Net::HTTPServerRequest& request );

    private:

        // httpserver
        KFHttpServer* _http_server;
    };
}


#endif
