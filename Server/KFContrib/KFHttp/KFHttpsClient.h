#ifndef __KF_HTTPS_CLIENT_H__
#define __KF_HTTPS_CLIENT_H__

#include "KFHttp.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPSClientSession.h"

namespace KFrame
{
    class KFHttpsClient : public KFHttp
    {
    public:
        KFHttpsClient();
        virtual ~KFHttpsClient();

        static void Initialize();
        //////////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual Poco::Net::HTTPClientSession* GetHttpSession();

    protected:

        static Poco::Net::Context::Ptr _context;
        Poco::Net::HTTPSClientSession* _https_session;
    };
}


#endif