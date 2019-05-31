#include "KFHttpClient.h"
#include "Poco/Net/HTTPClientSession.h"

namespace KFrame
{
    KFHttpClient::KFHttpClient()
    {
        _http_session = __KF_NEW__( Poco::Net::HTTPClientSession );
    }

    KFHttpClient::~KFHttpClient()
    {
        __KF_DELETE__( Poco::Net::HTTPClientSession, _http_session );
    }

    Poco::Net::HTTPClientSession* KFHttpClient::GetHttpSession()
    {
        return _http_session;
    }
}

