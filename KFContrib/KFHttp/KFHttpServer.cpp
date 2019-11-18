#include "KFHttpServer.h"
#include "KFHttpFactory.h"
#include "KFHttp/KFHttpCommon.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SecureServerSocket.h"

namespace KFrame
{
    KFHttpServer::KFHttpServer()
    {
        _http_server = nullptr;
    }

    KFHttpServer::~KFHttpServer()
    {
        __DELETE_OBJECT__( _http_server );
    }

    void KFHttpServer::ShutDown()
    {
        _http_server->stopAll();

        KFLocker kflocker( _kf_mutex );
        for ( auto kfdata : _kf_function_data )
        {
            __KF_DELETE__( KFFunctionData, kfdata );
        }
        _kf_function_data.clear();
    }

    void KFHttpServer::Start( const std::string& ip, uint32 port, uint32 maxthread, uint32 maxqueue, uint32 idletime, bool keeplive )
    {
        try
        {
            Poco::Net::HTTPServerParams* params = new Poco::Net::HTTPServerParams();
            params->setKeepAlive( keeplive );
            params->setMaxThreads( maxthread );
            params->setMaxQueued( maxqueue );
            params->setThreadIdleTime( idletime );

            Poco::Net::SocketAddress address( ip, port );
            Poco::Net::ServerSocket socket( address );

            auto httpfactory = new KFHttpFactory( this );
            _http_server = new Poco::Net::HTTPServer( httpfactory, socket, params );
            _http_server->start();
        }
        catch ( Poco::Exception& exc )
        {
            __LOG_ERROR__( "init http server[{}:{}] failed[{}:{}]!",
                           ip, port, exc.code(), exc.message() );
        }
    }

    void KFHttpServer::RegisterMethonFunction( const std::string& url, bool sync, KFHttpMethodFunction& function )
    {
        auto kfhttpfunction = _functions.Create( url );
        kfhttpfunction->_sync = sync;
        kfhttpfunction->_function = function;
    }

    void KFHttpServer::UnRegisterMethonFunction( const std::string& url )
    {
        _functions.Remove( url );
    }

    std::string KFHttpServer::ProcessHttpRequest( const std::string& url, const std::string& ip, const std::string& data )
    {
        auto kffunction = _functions.Find( url );
        if ( kffunction == nullptr )
        {
            return _invalid_string;
        }

        if ( !kffunction->_sync )
        {
            auto strdata = _invalid_string;
            try
            {
                strdata = kffunction->_function( ip, data );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "http function failed! data=[{}]", data );
            }

            return strdata;
        }

        auto kfdata = __KF_NEW__( KFFunctionData );
        kfdata->_ip = ip;
        kfdata->_data = data;
        kfdata->_kf_function = kffunction;
        {
            // 保存数据
            KFLocker kflocker( _kf_mutex );
            _kf_function_data.push_back( kfdata );
        }

        return KFHttpCommon::SendResponseCode( KFEnum::Ok );
    }

    void KFHttpServer::Run()
    {
        std::list< KFFunctionData* > templist;
        {
            KFLocker kflocker( _kf_mutex );
            templist.swap( _kf_function_data );
        }

        for ( auto kfdata : templist )
        {
            try
            {
                kfdata->_kf_function->_function( kfdata->_ip, kfdata->_data );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "http function failed! data=[{}]", kfdata->_data );
            }

            __KF_DELETE__( KFFunctionData, kfdata );
        }
    }
}