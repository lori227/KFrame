#include "KFHttpHandle.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "KFHttpServer.h"

namespace KFrame
{
    KFHttpHandle::KFHttpHandle( KFHttpServer* http_server )
    {
        _http_server = http_server;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpHandle::handleRequest( Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response )
    {
        auto& stream = request.stream();
        std::ostringstream os;
        stream >> os.rdbuf();
        auto data = os.str();

        auto url = request.getURI();
        KFUtility::SplitString( url, "/" );

        __LOG_DEBUG__( "url=[{}] data=[{}]", url, data );
        if ( url.empty() )
        {
            return;
        }

        auto& address = request.clientAddress();
        auto ip = address.host().toString();
        auto result_data = _http_server->ProcessHttpRequest( url, ip, data );

        __LOG_DEBUG__( "response=[{}]", result_data );

        // 发回给客户端
        auto& send = response.send();
        send << result_data;
        send.flush();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

}