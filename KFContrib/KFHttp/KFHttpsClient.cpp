#include "KFHttpsClient.h"
#include "Poco/SharedPtr.h"
#include "Poco/URI.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTTPSClientSession.h"

namespace KFrame
{
    using namespace Poco::Net;
    static Poco::Net::Context::Ptr _context = nullptr;

    KFHttpsClient::KFHttpsClient()
    {
        _https_session = new Poco::Net::HTTPSClientSession( _context );
    }

    void KFHttpsClient::Initialize()
    {
        Poco::Net::initializeSSL();
        _context = __KF_NEW__( Context, Context::CLIENT_USE, "", Context::VERIFY_RELAXED, 9, true );
        _context->disableProtocols( Context::PROTO_SSLV2 | Context::PROTO_SSLV3 );

        Poco::SharedPtr<InvalidCertificateHandler> pCert = new AcceptCertificateHandler( false );
        Poco::Net::SSLManager::instance().initializeClient( 0, pCert, _context );
    }

    KFHttpsClient::~KFHttpsClient()
    {
        delete _https_session;
    }

    Poco::Net::HTTPClientSession* KFHttpsClient::GetHttpSession()
    {
        return _https_session;
    }

}

