#include "KFHttp.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/URI.h"
#include "Poco/StreamCopier.h"
#include "Poco/ThreadPool.h"
#include "Poco/Exception.h"

namespace KFrame
{
	KFHttp::KFHttp()
	{
	}

	KFHttp::~KFHttp()
	{

	}

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

	std::string KFHttp::RunHttp( const std::string& url, const std::string& data )
	{
		Poco::Net::HTTPRequest request;
		request.setContentType( "application/json" );
		request.setVersion( Poco::Net::HTTPRequest::HTTP_1_1 );
		request.setMethod( Poco::Net::HTTPRequest::HTTP_GET );

		Poco::URI verifyuri( url );
		request.setURI( GetURI( url, verifyuri ) );
		return HttpRequest( request, verifyuri, data );
	}

#define MAX_EXCEPTION 3
	std::string KFHttp::HttpRequest( Poco::Net::HTTPRequest& request, Poco::URI& url, const std::string& data )
	{
		uint32 exceptioncount = 0;

		Poco::Net::HTTPClientSession* psession = GetHttpSession();
		psession->setHost( url.getHost() );
		psession->setPort( url.getPort() );

		std::string result;
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
				psession->reset();
				++exceptioncount;
			}
			catch( ... )
			{
				break;
			}
		} while ( exceptioncount < MAX_EXCEPTION );

		psession->reset();
		return result;
	}
}
