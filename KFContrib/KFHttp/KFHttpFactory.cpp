#include "KFHttpFactory.h"
#include "KFHttpServer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "KFHttpHandle.h"

namespace KFrame
{
    KFHttpFactory::KFHttpFactory( KFHttpServer* http_server )
    {
        _http_server = http_server;
    }

    Poco::Net::HTTPRequestHandler* KFHttpFactory::createRequestHandler( const Poco::Net::HTTPServerRequest& request )
    {
        return new KFHttpHandle( _http_server );
    }
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////
