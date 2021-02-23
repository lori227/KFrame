#include "KFHttpThread.hpp"
#include "KFHttp/KFHttp.h"

namespace KFrame
{
    KFHttpThread::KFHttpThread()
    {
        _thread_run = true;
        _http_response_list.InitQueue( 20000u, 20000u );
        KFThread::Create( this, &KFHttpThread::RunHttpRequest, __FUNC_LINE__ );
    }

    KFHttpThread::~KFHttpThread()
    {
        _thread_run = false;
        {
            KFLocker locker( _kf_req_mutex );
            _http_request_list.clear();
        }
    }

    void KFHttpThread::ShutDown()
    {
        _thread_run = false;
    }

    void KFHttpThread::AddHttpRequest( std::shared_ptr<KFHttpData> http_data )
    {
        KFLocker locker( _kf_req_mutex );
        _http_request_list.push_back( http_data );
    }

    void KFHttpThread::RunHttpRequest()
    {
        while ( _thread_run )
        {
            std::list<std::shared_ptr<KFHttpData>> temp_list;
            {
                KFLocker locker( _kf_req_mutex );
                temp_list.swap( _http_request_list );
            }

            for ( auto http_data : temp_list )
            {
                http_data->Request();
                if ( http_data->_function != nullptr )
                {
                    _http_response_list.Push( http_data );
                }
            }

            KFThread::Sleep( 1 );
        }
    }

    void KFHttpThread::RunHttpResponse()
    {
        auto responsecount = 0u;
        do
        {
            auto http_data = _http_response_list.Pop();
            if ( http_data == nullptr )
            {
                break;
            }

            ++responsecount;
            http_data->Response();
        } while ( responsecount <= 200u );
    }

}
