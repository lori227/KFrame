#include "KFHttpClientModule.hpp"
#include "KFHttp/KFHttpCommon.h"
#include "KFHttp/KFHttpClient.h"
#include "KFHttp/KFHttpsClient.h"

namespace KFrame
{
    KFHttpClientModule::~KFHttpClientModule()
    {
        for ( auto httpthread : _http_thread_list )
        {
            __KF_DELETE__( KFHttpThread, httpthread );
        }
        _http_thread_list.clear();
    }

    void KFHttpClientModule::BeforeRun()
    {
        KFHttpsClient::Initialize();
    }

    void KFHttpClientModule::BeforeShut()
    {
        for ( auto httpthread : _http_thread_list )
        {
            httpthread->ShutDown();
        }
    }

    void KFHttpClientModule::Run()
    {
        for ( auto httpthread : _http_thread_list )
        {
            httpthread->RunHttpResponse();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientModule::SendCode( uint32 code )
    {
        return KFHttpCommon::SendResponseCode( code );
    }

    uint32 KFHttpClientModule::GetCode( KFJson& json )
    {
        return KFHttpCommon::GetResponseCode( json );
    }

    std::string KFHttpClientModule::SendResponse( KFJson& json )
    {
        return KFHttpCommon::SendResponse( json );
    }

    bool KFHttpClientModule::IsHttpsClient( const std::string& url )
    {
        return url.compare( 0, 5, "https" ) == 0;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientModule::STGet( const std::string& url, const std::string& data )
    {
        return STRequest( KFHttp::Get, url, data );
    }

    std::string KFHttpClientModule::STPost( const std::string& url, const std::string& data )
    {
        return STRequest( KFHttp::Post, url, data );
    }

    std::string KFHttpClientModule::STGet( const std::string& url, KFJson& json )
    {
        auto data = __JSON_SERIALIZE__( json );
        return STRequest( KFHttp::Get, url, data );
    }

    std::string KFHttpClientModule::STPost( const std::string& url, KFJson& json )
    {
        auto data = __JSON_SERIALIZE__( json );
        return STRequest( KFHttp::Post, url, data );
    }

    std::string KFHttpClientModule::STRequest( uint32 type, const std::string& url, const std::string& data )
    {
        if ( IsHttpsClient( url ) )
        {
            KFHttpsClient httpclient;
            return httpclient.RunHttp( type, url, data );
        }

        KFHttpClient httpclient;
        return httpclient.RunHttp( type, url, data );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpClientModule::MTGetRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args )
    {
        MTRequest( KFHttp::Get, function, url, data, _invalid_string );
    }

    void KFHttpClientModule::MTGetRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args )
    {
        auto data = __JSON_SERIALIZE__( json );
        MTRequest( KFHttp::Get, function, url, data, _invalid_string );
    }

    void KFHttpClientModule::MTPostRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args )
    {
        MTRequest( KFHttp::Post, function, url, data, args );
    }

    void KFHttpClientModule::MTPostRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& args )
    {
        auto data = __JSON_SERIALIZE__( json );
        MTRequest( KFHttp::Post, function, url, data, args );
    }

    void KFHttpClientModule::MTRequest( uint32 type, KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& args )
    {
        auto httpdata = __KF_NEW__( KFHttpData );
        httpdata->_url = url;
        httpdata->_type = type;
        httpdata->_data = data;
        httpdata->_args = args;
        httpdata->_function = function;

        if ( IsHttpsClient( url ) )
        {
            httpdata->_http = __KF_NEW__( KFHttpsClient );
        }
        else
        {
            httpdata->_http = __KF_NEW__( KFHttpClient );
        }

        AddHttpData( httpdata );
    }

    void KFHttpClientModule::AddHttpData( KFHttpData* httpdata )
    {
        // 先创建http异步线程
        CreateHttpThread();

        if ( _thread_index >= ( uint32 )_http_thread_list.size() )
        {
            _thread_index = 0u;
        }

        auto httpthread = _http_thread_list[ _thread_index++ ];
        httpthread->AddHttpRequest( httpdata );
    }

    void KFHttpClientModule::CreateHttpThread()
    {
        if ( !_http_thread_list.empty() )
        {
            return;
        }

        // 创建4个线程来执行异步http逻辑
        for ( auto i = 0u; i < 4u; ++i )
        {
            _http_thread_list.push_back( __KF_NEW__( KFHttpThread ) );
        }
    }
}
