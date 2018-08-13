#include "KFHttpClientManage.h"
#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"

namespace KFrame
{
    KFHttpClientManage::KFHttpClientManage()
    {
        _thread_run = false;
    }

    KFHttpClientManage::~KFHttpClientManage()
    {

    }

    void KFHttpClientManage::Initialize()
    {
        KFHttpsClient::Initialize();

        _req_http_data.InitQueue( 1000 );
        _ack_http_data.InitQueue( 1000 );
    }

    void KFHttpClientManage::ShutDown()
    {
        _thread_run = false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string KFHttpClientManage::SendSTHttp( const std::string& url, const std::string& data )
    {
        KFHttpClient httpclient;
        return httpclient.RunHttp( url, data );
    }

    std::string KFHttpClientManage::SendSTHttps( const std::string& url, const std::string& data )
    {
        KFHttpsClient httpclient;
        return httpclient.RunHttp( url, data );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFHttpClientManage::SendMTHttp( const std::string& url, const std::string& data, KFHttpClientFunction& function )
    {
        auto httpdata = __KF_CREATE__( KFHttpData );
        httpdata->_url = url;
        httpdata->_data = data;
        httpdata->_function = function;
        httpdata->_http = __KF_CREATE__( KFHttpClient );
        AddHttpData( httpdata );
    }

    void KFHttpClientManage::SendMTHttps( const std::string& url, const std::string& data, KFHttpClientFunction& function )
    {
        auto httpdata = __KF_CREATE__( KFHttpData );
        httpdata->_url = url;
        httpdata->_data = data;
        httpdata->_function = function;
        httpdata->_http = __KF_CREATE__( KFHttpsClient );
        AddHttpData( httpdata );
    }

    void KFHttpClientManage::AddHttpData( KFHttpData* httpdata )
    {
        if ( !_thread_run )
        {
            _thread_run = true;
            KFThread::CreateThread( this, &KFHttpClientManage::RunHttpRequest, __FUNC_LINE__ );
        }

        _req_http_data.PushObject( httpdata );
    }

    void KFHttpClientManage::RunUpdate()
    {
        auto httpdata = _ack_http_data.PopObject();
        while ( httpdata != nullptr )
        {
            httpdata->Finish();
            __KF_DESTROY__( KFHttpData, httpdata );
            httpdata = _ack_http_data.PopObject();
        }
    }

    void KFHttpClientManage::RunHttpRequest()
    {
        do
        {
            auto httpdata = _req_http_data.PopObject();
            while ( httpdata != nullptr )
            {
                httpdata->Request();
                _ack_http_data.PushObject( httpdata );

                httpdata = _req_http_data.PopObject();
            }

            KFThread::Sleep( 1 );
        } while ( _thread_run );
    }
}