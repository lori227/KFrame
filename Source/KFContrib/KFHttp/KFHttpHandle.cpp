#include "KFHttpHandle.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "KFHttpServer.h"

namespace KFrame
{
	KFHttpHandle::KFHttpHandle( KFHttpServer* kfhttpserver )
	{
		_http_server = kfhttpserver;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KFHttpHandle::handleRequest( Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response )
	{
		std::string resultdata;

		try
		{
			std::string url = request.getURI();
			std::string data = ReadRequestData( request );

			KFUtility::SplitString( url, "/" );

			auto& address = request.clientAddress();
			auto ip = address.host().toString();
			resultdata = _http_server->ProcessHttpRequest( url, ip, data );

			// 发回给客户端
			auto& send = response.send();
			send << resultdata;
			send.flush();
		}
		catch ( ... )
		{
			
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFHttpHandle::ReadRequestData( Poco::Net::HTTPServerRequest &request )
	{
		auto& stream = request.stream();

		std::ostringstream os;
		stream >> os.rdbuf();
	
		std::string data;
		data = os.str();
		return data;
	}

}