#include "KFHttpClientModule.hpp"
#include "KFHttp/KFHttpCommon.h"
#include "KFHttp/KFHttpClient.h"
#include "KFHttp/KFHttpsClient.h"

namespace KFrame
{
    KFHttpClientModule::~KFHttpClientModule()
    {
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
    void KFHttpClientModule::MTGetRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& pass_back_data )
    {
        MTRequest( KFHttp::Get, function, url, data, pass_back_data );
    }

    void KFHttpClientModule::MTGetRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& pass_back_data )
    {
        auto data = __JSON_SERIALIZE__( json );
        MTRequest( KFHttp::Get, function, url, data, pass_back_data );
    }

    void KFHttpClientModule::MTPostRequest( KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& pass_back_data )
    {
        MTRequest( KFHttp::Post, function, url, data, pass_back_data );
    }

    void KFHttpClientModule::MTPostRequest( KFHttpClientFunction& function, const std::string& url, KFJson& json, const std::string& pass_back_data )
    {
        auto data = __JSON_SERIALIZE__( json );
        MTRequest( KFHttp::Post, function, url, data, pass_back_data );
    }

    void KFHttpClientModule::MTRequest( uint32 type, KFHttpClientFunction& function, const std::string& url, const std::string& data, const std::string& pass_back_data )
    {
        auto http_data = __MAKE_SHARED__( KFHttpData );
        http_data->_url = url;
        http_data->_type = type;
        http_data->_data = data;
        http_data->_pass_back_data = pass_back_data;
        http_data->_function = function;

        if ( IsHttpsClient( url ) )
        {
            http_data->_http = __MAKE_SHARED__( KFHttpsClient );
        }
        else
        {
            http_data->_http = __MAKE_SHARED__( KFHttpClient );
        }

        AddHttpData( http_data );
    }

    void KFHttpClientModule::AddHttpData( std::shared_ptr<KFHttpData> http_data )
    {
        // 先创建http异步线程
        CreateHttpThread();

        auto index = _thread_index++;
        if ( index >= ( uint32 )_http_thread_list.size() )
        {
            index = 0u;
        }

        auto httpthread = _http_thread_list[ index ];
        httpthread->AddHttpRequest( http_data );
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
            _http_thread_list.push_back( __MAKE_SHARED__( KFHttpThread ) );
        }
    }
}
