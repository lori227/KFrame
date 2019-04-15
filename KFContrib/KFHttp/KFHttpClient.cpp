#include "KFHttpClient.h"

namespace KFrame
{
    KFHttpClient::KFHttpClient()
    {
    }

    KFHttpClient::~KFHttpClient()
    {

    }

    Poco::Net::HTTPClientSession* KFHttpClient::GetHttpSession()
    {
        return &_http_session;
    }
}

