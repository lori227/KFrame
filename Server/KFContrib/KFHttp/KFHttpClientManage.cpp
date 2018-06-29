#include "KFHttpClientManage.h"
#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"

namespace KFrame
{
    KFHttpClientManage::KFHttpClientManage()
    {
        _req_http_data_count = 0;
    }

    KFHttpClientManage::~KFHttpClientManage()
    {
        {
            KFLocker lock( _finish_mutex );
            for ( auto httpdata : _finish_http_data )
            {
                __KF_DESTROY__( KFHttpData, httpdata );
            }
        }

        for ( auto httpdata : _wait_http_data )
        {
            __KF_DESTROY__( KFHttpData, httpdata );
        }
    }

    void KFHttpClientManage::Initialize()
    {
        KFHttpsClient::Initialize();
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
        _wait_http_data.push_back( httpdata );
    }

    void KFHttpClientManage::SendMTHttps( const std::string& url, const std::string& data, KFHttpClientFunction& function )
    {
        auto httpdata = __KF_CREATE__( KFHttpData );
        httpdata->_url = url;
        httpdata->_data = data;
        httpdata->_function = function;
        httpdata->_http = __KF_CREATE__( KFHttpsClient );
        _wait_http_data.push_back( httpdata );
    }

    void KFHttpClientManage::AddFinishHttp( KFHttpData* httpdata )
    {
        KFLocker lock( _finish_mutex );
        _finish_http_data.push_back( httpdata );
    }

    void KFHttpClientManage::RunUpdate()
    {
        // 结束的http
        ExecuteFinishHttp();

        // 等待的http
        ExecuteWaitHttp();
    }

    void KFHttpClientManage::ExecuteFinishHttp()
    {
        HttpDataList temphttpdata;
        {
            KFLocker lock( _finish_mutex );
            if ( _finish_http_data.empty() )
            {
                return;
            }

            temphttpdata.swap( _finish_http_data );
        }

        for ( auto httpdata : temphttpdata )
        {
            httpdata->Finish();
            __KF_DESTROY__( KFHttpData, httpdata );

            _req_http_data_count -= __MIN__( _req_http_data_count, 1 );
        }
    }

#define __MAX_REQ_HTTP_DATA_COUNT__ 16

    void KFHttpClientManage::ExecuteWaitHttp()
    {
        do
        {
            if ( _wait_http_data.empty() )
            {
                break;
            }

            // 判断当前请求的数量
            if ( _req_http_data_count >= __MAX_REQ_HTTP_DATA_COUNT__ )
            {
                break;
            }

            auto httpdata = _wait_http_data.front();
            _wait_http_data.pop_front();

            httpdata->StartMTHttp( this );
            ++_req_http_data_count;
        } while ( true );
    }
}