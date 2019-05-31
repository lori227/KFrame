#ifndef __KF_HTTPS_CLIENT_H__
#define __KF_HTTPS_CLIENT_H__

#include "KFHttp.h"

namespace Poco
{
    namespace Net
    {
        class HTTPSClientSession;
    }
}

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

        Poco::Net::HTTPSClientSession* _https_session;
    };
}


#endif