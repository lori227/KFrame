#include "KFHttpClientModule.h"
#include "KFHttp/KFHttpCommon.h"

#include "KFJson.h"

namespace KFrame
{
    KFHttpClientModule::KFHttpClientModule()
    {
        _thread_run = false;
    }

    KFHttpClientModule::~KFHttpClientModule()
    {
        {
            KFLocker locker( _kf_req_mutex );
            for ( auto httpdata : _req_http_data )
            {
                __KF_DESTROY__( KFHttpData, httpdata );
            }
            _req_http_data.clear();
        }

        {
            KFLocker locker( _kf_ack_mutex );
            for ( auto httpdata : _ack_http_data )
            {
                __KF_DESTROY__( KFHttpData, httpdata );
            }
            _ack_http_data.clear();
        }
    }

    void KFHttpClientModule::BeforeRun()
    {
        KFHttpsClient::Initialize();
    }

    void KFHttpClientModule::BeforeShut()
    {
        _thread_run = false;
    }

    void KFHttpClientModule::Run()
    {
        std::list< KFHttpData* > templist;
        {
            KFLocker locker( _kf_ack_mutex );
            templist.swap( _ack_http_data );
        }

        for ( auto httpdata : templist )
        {
            httpdata->Response();
            __KF_DESTROY__( KFHttpData, httpdata );
        }
    }

    void KFHttpClientModule::RunHttpRequest()
    {
        while ( _thread_run )
        {
            std::list< KFHttpData* > templist;
            {
                KFLocker locker( _kf_req_mutex );
                templist.swap( _req_http_data );
            }

            for ( auto httpdata : templist )
            {
                httpdata->Request();

                KFLocker locker( _kf_ack_mutex );
                _ack_http_data.push_back( httpdata );
            }

            KFThread::Sleep( 1 );
        }
    }

    bool KFHttpClientModule::IsHttpsClient( const std::string& url )
    {
        return url.compare( 0, 5, "https" ) == 0;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientModule::StartSTHttpClient( const std::string& url, const std::string& data )
    {
        if ( IsHttpsClient( url ) )
        {
            KFHttpsClient httpclient;
            return httpclient.RunHttp( url, data );
        }

        KFHttpClient httpclient;
        return httpclient.RunHttp( url, data );
    }

    std::string KFHttpClientModule::StartSTHttpClient( const std::string& url, KFJson& json )
    {
        auto temp = json.Serialize();
        return StartSTHttpClient( url, temp );
    }
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpClientModule::StartMTHttpClient( const std::string& url, const std::string& data )
    {
        static KFHttpClientFunction _null_function = nullptr;
        StartMTHttpClient( _null_function, url, data, _invalid_str );
    }

    void KFHttpClientModule::StartMTHttpClient( const std::string& url, KFJson& json )
    {
        static KFHttpClientFunction _null_function = nullptr;
        StartMTHttpClient( _null_function, url, json, _invalid_str );
    }

    void KFHttpClientModule::StartMTHttpClient( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& callback )
    {
        auto data = json.Serialize();
        StartMTHttpClient( function, url, data, callback );
    }

    void KFHttpClientModule::StartMTHttpClient( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& callback )
    {
        auto httpdata = __KF_CREATE__( KFHttpData );
        httpdata->_url = url;
        httpdata->_data = data;
        httpdata->_callback = callback;
        httpdata->_function = function;

        if ( IsHttpsClient( url ) )
        {
            httpdata->_http = __KF_CREATE__( KFHttpsClient );
        }
        else
        {
            httpdata->_http = __KF_CREATE__( KFHttpClient );
        }

        AddHttpData( httpdata );
    }

    void KFHttpClientModule::AddHttpData( KFHttpData* httpdata )
    {
        if ( !_thread_run )
        {
            _thread_run = true;
            KFThread::CreateThread( this, &KFHttpClientModule::RunHttpRequest, __FUNC_LINE__ );
        }

        KFLocker locker( _kf_req_mutex );
        _req_http_data.push_back( httpdata );
    }
}
