#include "KFHttp.h"


namespace KFrame
{
    std::string KFHttp::GetURI( const std::string& url, Poco::URI& pocouri )
    {
        auto path = pocouri.getPath();
        auto query = pocouri.getQuery();

        if ( query.empty() )
        {
            return path;
        }

        return path + "?" + query;
    }

#define MAX_EXCEPTION 3

    std::string KFHttp::RunHttp( const std::string& url, const std::string& data )
    {
        __LOG_DEBUG__( "http url={}", url );
        if ( !data.empty() )
        {
            __LOG_DEBUG__( "http data={}", data );
        }

        Poco::Net::HTTPRequest request;
        request.setContentType( "application/json" );
        request.setVersion( Poco::Net::HTTPRequest::HTTP_1_1 );
        request.setMethod( Poco::Net::HTTPRequest::HTTP_GET );

        Poco::URI verifyuri( url );
        request.setURI( GetURI( url, verifyuri ) );

        Poco::Net::HTTPClientSession* psession = GetHttpSession();
        psession->setHost( verifyuri.getHost() );
        psession->setPort( verifyuri.getPort() );

        std::string result;
        uint32 exceptioncount = 0;

        do
        {
            try
            {
                request.setContentLength( data.length() );
                psession->sendRequest( request ) << data;

                Poco::Net::HTTPResponse response;
                auto& recvstream = psession->receiveResponse( response );

                std::ostringstream os;
                recvstream >> os.rdbuf();
                result = os.str();

                break;
            }
            catch ( Poco::Exception& exception )
            {
                if ( exceptioncount == 0 )
                {
                    __LOG_ERROR__( "[{}]http failed, error = {}, code = {}", verifyuri.toString(), exception.what(), exception.code() );
                }
                ++exceptioncount;
            }
        } while ( exceptioncount < MAX_EXCEPTION );

        psession->reset();
        __LOG_DEBUG__( "http result={}", result );

        return result;
    }

}
