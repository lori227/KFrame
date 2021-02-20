#include "KFHttp.h"


namespace KFrame
{
    std::string KFHttp::GetURI( const std::string& url, Poco::URI& poco_uri )
    {
        auto path = poco_uri.getPath();
        auto query = poco_uri.getQuery();

        if ( query.empty() )
        {
            return path;
        }

        return path + "?" + query;
    }

#define MAX_EXCEPTION 3

    std::string KFHttp::RunHttp( uint32 type, const std::string& url, const std::string& data )
    {
        __LOG_DEBUG__( "http url=[{}]", url );
        if ( !data.empty() )
        {
            __LOG_DEBUG__( "http data=[{}]", data );
        }

        Poco::Net::HTTPRequest request;
        request.setContentType( "application/json" );
        request.setVersion( Poco::Net::HTTPRequest::HTTP_1_1 );
        if ( type == KFHttp::Post )
        {
            request.setMethod( Poco::Net::HTTPRequest::HTTP_POST );
        }
        else
        {
            request.setMethod( Poco::Net::HTTPRequest::HTTP_GET );
        }

        Poco::URI verify_uri( url );
        request.setURI( GetURI( url, verify_uri ) );

        Poco::Net::HTTPClientSession* session = GetHttpSession();
        session->setHost( verify_uri.getHost() );
        session->setPort( verify_uri.getPort() );

        std::string result;
        uint32 exception_count = 0;

        do
        {
            try
            {
                request.setContentLength( data.length() );
                session->sendRequest( request ) << data;

                Poco::Net::HTTPResponse response;
                auto& recv_stream = session->receiveResponse( response );

                std::ostringstream os;
                recv_stream >> os.rdbuf();
                result = os.str();

                break;
            }
            catch ( Poco::Exception& exception )
            {
                if ( exception_count == 0 )
                {
                    __LOG_ERROR__( "[{}]http failed, error = {}, code = {}", verify_uri.toString(), exception.what(), exception.code() );
                }
                ++exception_count;
            }
        } while ( exception_count < MAX_EXCEPTION );

        session->reset();
        __LOG_DEBUG__( "http result={}", result );

        return result;
    }

}
