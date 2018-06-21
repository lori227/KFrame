#include "KFHttpsClient.h"
#include "Poco/SharedPtr.h"
#include "Poco/URI.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"

namespace KFrame
{
	using namespace Poco::Net;
	Context::Ptr KFHttpsClient::_context = nullptr;

	KFHttpsClient::KFHttpsClient()
	{
		_https_session = __KF_NEW__( Poco::Net::HTTPSClientSession, _context );
	}

	void KFHttpsClient::Initialize()
	{
		Poco::Net::initializeSSL();

		Poco::SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler( false );
		_context = __KF_NEW__( Context, Context::CLIENT_USE, "", Context::VERIFY_RELAXED, 9, true );
		_context->disableProtocols( Context::PROTO_SSLV2 | Context::PROTO_SSLV3 );
		Poco::Net::SSLManager::instance().initializeClient( 0, pCert, _context );
	}

	KFHttpsClient::~KFHttpsClient()
	{
		__KF_DELETE__( _https_session );
	}

	Poco::Net::HTTPClientSession* KFHttpsClient::GetHttpSession()
	{
		return _https_session;
	}

}

