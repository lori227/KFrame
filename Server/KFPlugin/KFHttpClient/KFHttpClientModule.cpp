#include "KFHttpClientModule.h"
#include "KFHttp/KFHttpClientManage.h"
#include "KFHttp/KFHttpCommon.h"
#include "KFJson.h"

namespace KFrame
{
    KFHttpClientModule::KFHttpClientModule()
    {
        _kf_http_manage = new KFHttpClientManage();
    }

    KFHttpClientModule::~KFHttpClientModule()
    {
        delete _kf_http_manage;
    }

    void KFHttpClientModule::BeforeRun()
    {
        _kf_http_manage->Initialize();
    }

    void KFHttpClientModule::ShutDown()
    {
        _kf_http_manage->ShutDown();
    }

    void KFHttpClientModule::Run()
    {
        _kf_http_manage->RunUpdate();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientModule::SendResponseCode( uint32 code )
    {
        return KFHttpCommon::SendResponseCode( code );
    }

    uint32 KFHttpClientModule::GetResponseCode( KFJson& json )
    {
        return KFHttpCommon::GetResponseCode( json );
    }

    std::string KFHttpClientModule::SendResponse( KFJson& json )
    {
        return KFHttpCommon::SendResponse( json );
    }

    void KFHttpClientModule::MakeSignature( KFJson& json )
    {
        KFHttpCommon::MakeSignature( json );
    }

    bool KFHttpClientModule::VerifySignature( KFJson& json )
    {
        return KFHttpCommon::VerifySignature( json );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFHttpClientModule::StartMTHttpClient( const std::string& url, const std::string& data )
    {
        static KFHttpClientFunction _null_function = nullptr;
        _kf_http_manage->SendMTHttp( url, data, _null_function );
    }

    void KFHttpClientModule::StartMTHttpClient( const std::string& url, KFJson& json, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto data = json.Serialize();
        StartMTHttpClient( url, data );
    }

    void KFHttpClientModule::StartMTHttpClient( const std::string& url, const std::string& data, KFHttpClientFunction& function )
    {
        _kf_http_manage->SendMTHttp( url, data, function );
    }

    void KFHttpClientModule::StartMTHttpClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto data = json.Serialize();
        _kf_http_manage->SendMTHttp( url, data, function );
    }

    void KFHttpClientModule::StartMTHttpsClient( const std::string& url, KFJson& json, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto data = json.Serialize();
        StartMTHttpsClient( url, data );
    }

    void KFHttpClientModule::StartMTHttpsClient( const std::string& url, const std::string& data )
    {
        static KFHttpClientFunction _null_function = nullptr;
        _kf_http_manage->SendMTHttps( url, data, _null_function );
    }

    void KFHttpClientModule::StartMTHttpsClient( const std::string& url, const std::string& data, KFHttpClientFunction& function )
    {
        _kf_http_manage->SendMTHttps( url, data, function );
    }

    void KFHttpClientModule::StartMTHttpsClient( const std::string& url, KFJson& json, KFHttpClientFunction& function, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto data = json.Serialize();
        _kf_http_manage->SendMTHttps( url, data, function );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientModule::StartSTHttpsClient( const std::string& url, const std::string& data )
    {
        return _kf_http_manage->SendSTHttps( url, data );
    }

    std::string KFHttpClientModule::StartSTHttpsClient( const std::string& url, KFJson& json, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto temp = json.Serialize();
        return StartSTHttpsClient( url, temp );
    }

    std::string KFHttpClientModule::StartSTHttpClient( const std::string& url, const std::string& data )
    {
        return _kf_http_manage->SendSTHttp( url, data );
    }

    std::string KFHttpClientModule::StartSTHttpClient( const std::string& url, KFJson& json, bool sign )
    {
        if ( sign )
        {
            MakeSignature( json );
        }

        auto temp = json.Serialize();
        return StartSTHttpClient( url, temp );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

}
