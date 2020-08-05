#include "KFHttpThread.hpp"
#include "KFHttp/KFHttp.h"

namespace KFrame
{
    KFHttpThread::KFHttpThread()
    {
        _thread_run = true;
        _http_response_list.InitQueue( 20000u );
        KFThread::CreateThread( this, &KFHttpThread::RunHttpRequest, __FUNC_LINE__ );
    }

    KFHttpThread::~KFHttpThread()
    {
        _thread_run = false;
        {
            KFLocker locker( _kf_req_mutex );
            for ( auto httpdata : _http_request_list )
            {
                __KF_DELETE__( KFHttpData, httpdata );
            }
            _http_request_list.clear();
        }
    }

    void KFHttpThread::ShutDown()
    {
        _thread_run = false;
    }

    void KFHttpThread::AddHttpRequest( KFHttpData* httpdata )
    {
        KFLocker locker( _kf_req_mutex );
        _http_request_list.push_back( httpdata );
    }

    void KFHttpThread::RunHttpRequest()
    {
        while ( _thread_run )
        {
            std::list< KFHttpData* > templist;
            {
                KFLocker locker( _kf_req_mutex );
                templist.swap( _http_request_list );
            }

            for ( auto httpdata : templist )
            {
                httpdata->Request();
                _http_response_list.PushObject( httpdata );
            }

            KFThread::Sleep( 1 );
        }
    }

    void KFHttpThread::RunHttpResponse()
    {
        auto responsecount = 0u;
        do
        {
            auto httpdata = _http_response_list.PopObject();
            if ( httpdata == nullptr )
            {
                break;
            }

            ++responsecount;
            httpdata->Response();
            __KF_DELETE__( KFHttpData, httpdata );
        } while ( responsecount <= 200u );
    }

}
