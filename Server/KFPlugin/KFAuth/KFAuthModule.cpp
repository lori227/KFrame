#include "KFAuthModule.h"
#include "KFJson.h"
#include "KFThread/KFThread.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFAuthModule::KFAuthModule()
    {
    }

    KFAuthModule::~KFAuthModule()
    {

    }

    void KFAuthModule::InitModule()
    {
    }

    void KFAuthModule::BeforeRun()
    {
        __REGISTER_HTTP_FUNCTION__( KFField::_auth, false, &KFAuthModule::HandleAuthChannelLogin );
        __REGISTER_HTTP_FUNCTION__( KFField::_activation, false, &KFAuthModule::HandleAuthActivation );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        __UNREGISTER_HTTP_FUNCTION__( KFField::_auth );
        __UNREGISTER_HTTP_FUNCTION__( KFField::_activation );
        ///////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFAuthModule::HandleAuthChannelLogin( const std::string& ip, const std::string& data )
    {
        auto authdata = _kf_channel->AuthChannelLogin( data );

        KFJson authjson( authdata );
        auto retcode = _kf_http_server->GetResponseCode( authjson );
        if ( retcode != KFMsg::Success )
        {
            return _kf_http_server->SendResponseCode( retcode );
        }

        authjson[ KFField::_ip ] = ip;

        auto threadid = KFThread::GetThreadID();
        std::string platformurl = _kf_connection->FindPlatformAddress( threadid ) + KFField::_login;
        return _kf_http_client->StartSTHttpClient( platformurl, authjson, true );
    }

    std::string KFAuthModule::HandleAuthActivation( const std::string& ip, const std::string& data )
    {
        KFJson kfjson( data );
        kfjson[ KFField::_ip ] = ip;

        auto threadid = KFThread::GetThreadID();
        std::string platformurl = _kf_connection->FindPlatformAddress( threadid ) + KFField::_activation;
        return _kf_http_client->StartSTHttpClient( platformurl, kfjson, true );
    }
}