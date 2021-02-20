#include "KFHttpServer.h"
#include "KFHttpFactory.h"
#include "KFHttp/KFHttpCommon.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SecureServerSocket.h"

namespace KFrame
{
    KFHttpServer::~KFHttpServer()
    {
        __DELETE_OBJECT__( _http_server );
    }

    void KFHttpServer::ShutDown()
    {
        _http_server->stopAll();

        {
            KFLocker locker( _kf_mutex );
            _function_data_list.clear();
        }
    }

    void KFHttpServer::Start( const std::string& ip, uint32 port, uint32 max_thread, uint32 max_queue, uint32 idle_time, bool keep_live )
    {
        try
        {
            Poco::Net::HTTPServerParams* params = __NEW_OBJECT__( Poco::Net::HTTPServerParams );
            params->setKeepAlive( keep_live );
            params->setMaxQueued( max_queue );
            params->setMaxThreads( max_thread );
            params->setThreadIdleTime( idle_time );

            Poco::Net::SocketAddress address( ip, port );
            Poco::Net::ServerSocket socket( address );

            auto http_factory = __NEW_OBJECT__( KFHttpFactory, this );
            _http_server = __NEW_OBJECT__( Poco::Net::HTTPServer, http_factory, socket, params );
            _http_server->start();
        }
        catch ( Poco::Exception& exc )
        {
            __LOG_ERROR__( "init http server[{}:{}] failed[{}:{}]",
                           ip, port, exc.code(), exc.message() );
        }
    }

    void KFHttpServer::RegisterMethodFunction( const std::string& url, bool sync, KFHttpMethodFunction& function )
    {
        auto http_function = _functions.Create( url );
        http_function->_sync = sync;
        http_function->_function = function;
    }

    void KFHttpServer::UnRegisterMethodFunction( const std::string& url )
    {
        _functions.Remove( url );
    }

    std::string KFHttpServer::ProcessHttpRequest( const std::string& url, const std::string& ip, const std::string& data )
    {
        auto function = _functions.Find( url );
        if ( function == nullptr )
        {
            return _invalid_string;
        }

        if ( !function->_sync )
        {
            auto result_data = _invalid_string;
            try
            {
                result_data = function->_function( ip, data );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "http function failed! data=[{}]", data );
            }

            return result_data;
        }

        auto function_data = __MAKE_SHARED__( KFFunctionData );
        function_data->_ip = ip;
        function_data->_data = data;
        function_data->_http_function = function;
        {
            // 保存数据
            KFLocker locker( _kf_mutex );
            _function_data_list.push_back( function_data );
        }

        return KFHttpCommon::SendResponseCode( KFEnum::Ok );
    }

    void KFHttpServer::Run()
    {
        std::list<std::shared_ptr<KFFunctionData>> temp_list;
        {
            KFLocker locker( _kf_mutex );
            temp_list.swap( _function_data_list );
        }

        for ( auto& function_data : temp_list )
        {
            try
            {
                function_data->_http_function->_function( function_data->_ip, function_data->_data );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "http function failed! data=[{}]", function_data->_data );
            }
        }
    }
}