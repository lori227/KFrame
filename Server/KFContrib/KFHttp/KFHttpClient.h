#ifndef __KF_HTTP_CLIENT_H__
#define __KF_HTTP_CLIENT_H__

#include "KFHttp.h"
#include "Poco/Net/HTTPClientSession.h"

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
        Poco::Net::HTTPClientSession _http_session;
    };
}


#endif