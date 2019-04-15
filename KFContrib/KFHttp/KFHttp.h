#ifndef __KF_HTTP_H__
#define __KF_HTTP_H__

#include "KFrame.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/ThreadPool.h"
#include "Poco/Exception.h"

namespace KFrame
{
    class KFHttp
    {
    public:
        enum HttpEnum
        {
            Get = 1,
            Post = 2,
        };
    public:
        KFHttp() = default;
        virtual ~KFHttp() = default;

        // 执行http访问
        virtual std::string RunHttp( uint32 type, const std::string& url, const std::string& data );

    protected:
        // 获得访问路径
        virtual std::string GetURI( const std::string& url, Poco::URI& pocouri );

        // 获得clientsession
        virtual Poco::Net::HTTPClientSession* GetHttpSession() = 0;
        //////////////////////////////////////////////////////////////////////////////////////
    };

}
#endif

