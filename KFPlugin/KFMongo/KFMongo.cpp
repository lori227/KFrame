#include "KFMongo.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    KFMongo::KFMongo()
    {
        _connection = __KF_NEW__( Connection );
        _factory = __KF_NEW__( Connection::SocketFactory );
    }

    KFMongo::~KFMongo()
    {
        __KF_DELETE__( Connection, _connection );
        __KF_DELETE__( Connection::SocketFactory, _factory );
    }

    void KFMongo::InitMongo( const KFMongoConnectOption* connect_option, const KFMongoConnnectData* connect_data )
    {
        _database = connect_option->_database;

        ///     mongodb://<user>:<password>@hostname.com:<port>/database-name?options
        _connect_data = __FORMAT__( "mongodb://{}:{}@{}:{}/{}?ssl={}&connectTimeoutMS={}&socketTimeoutMS={}&authMechanism={}",
                                    connect_option->_user, connect_option->_password, connect_data->_ip, connect_data->_port, connect_option->_database,
                                    connect_option->_use_ssl, connect_option->_connect_timeout, connect_option->_execute_timeout, connect_option->_auth_type );

        ConnectMongo();
    }

    void KFMongo::ShutDown()
    {
        _is_connected = false;
        _connection->disconnect();
    }

    bool KFMongo::IsConnected()
    {
        return _is_connected;
    }

    bool KFMongo::ConnectMongo()
    {
        try
        {
            _is_connected = true;
            _connection->connect( _connect_data, *_factory );
            __LOG_INFO__( "mongo[{}] connect ok", _connect_data );
        }
        catch ( Poco::Exception& ex )
        {
            _is_connected = false;
            __LOG_ERROR__( "mongo[{}] connect failed = [{}]", _connect_data, ex.displayText() );
        }

        return _is_connected;
    }

    bool KFMongo::CheckDisconnected( int32 code )
    {
        switch ( code )
        {
        case 2013:
            return true;
        default:
            break;
        }

        return false;
    }


    bool KFMongo::SendRequest( RequestMessage& request )
    {
        uint32 repeat_count = 0u;
        do
        {
            try
            {
                _connection->sendRequest( request );
                return true;
            }
            catch ( Poco::Exception& ex )
            {
                if ( !CheckDisconnected( ex.code() ) )
                {
                    __LOG_ERROR__( "mongo failed code=[{}] message=[{}]", ex.code(), ex.displayText() );
                    return false;
                }

                ConnectMongo();
            }
        } while ( ++repeat_count < 3 );

        __LOG_ERROR__( "mongo disconnected" );
        return false;
    }

    bool KFMongo::SendRequest( RequestMessage& request, ResponseMessage& response )
    {
        uint32 repeat_count = 0u;
        do
        {
            try
            {
                _connection->sendRequest( request, response );
                return true;
            }
            catch ( Poco::Exception& ex )
            {
                if ( !CheckDisconnected( ex.code() ) )
                {
                    __LOG_ERROR__( "mongo failed code=[{}] message=[{}]", ex.code(), ex.displayText() );
                    return false;
                }

                ConnectMongo();
            }
        } while ( ++repeat_count < 3 );

        __LOG_ERROR__( "mongo disconnected" );
        return false;
    }
}