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

    void KFHttpClientManage::Initialize()
    {
        KFHttpsClient::Initialize();
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

        KFLocker locker( _kf_req_mutex );
        _req_http_data.push_back( httpdata );
    }

    void KFHttpClientManage::RunHttpRequest()
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

    void KFHttpClientManage::RunUpdate()
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
}