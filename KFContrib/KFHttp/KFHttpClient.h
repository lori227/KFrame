#ifndef __KF_HTTP_CLIENT_H__
#define __KF_HTTP_CLIENT_H__

#include "KFHttp.h"

namespace Poco
{
    namespace Net
    {
        class HTTPClientSession;
    }
}

namespace KFrame
{
    class KFHttpClient : public KFHttp
    {
    public:
        KFHttpClient();
        virtual ~KFHttpClient();
        //////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual Poco::Net::HTTPClientSession* GetHttpSession();

    protected:
        Poco::Net::HTTPClientSession* _http_session;
    };
}


#endif