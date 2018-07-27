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
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ), false, &KFAuthModule::HandleAuthChannelLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ), false, &KFAuthModule::HandleAuthActivation );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ) );
        ///////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthChannelLogin )
    {
        auto authdata = _kf_channel->AuthChannelLogin( data );

        KFJson authjson( authdata );
        auto retcode = _kf_http_server->GetResponseCode( authjson );
        if ( retcode != KFMsg::Success )
        {
            return _kf_http_server->SendResponseCode( retcode );
        }

        authjson[ __KF_STRING__( ip ) ] = ip;

        auto threadid = KFThread::GetThreadID();
        std::string platformurl = _kf_ip_address->FindPlatformAddress( threadid ) + __KF_STRING__( login );
        return _kf_http_client->StartSTHttpClient( platformurl, authjson, true );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthActivation )
    {
        KFJson kfjson( data );
        kfjson[ __KF_STRING__( ip ) ] = ip;

        auto threadid = KFThread::GetThreadID();
        std::string platformurl = _kf_ip_address->FindPlatformAddress( threadid ) + __KF_STRING__( activation );
        return _kf_http_client->StartSTHttpClient( platformurl, kfjson, true );
    }
}