#include "KFMongo.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMongo::InitMongo( const KFMongoSetting* kfsetting )
    {
        ///     mongodb://<user>:<password>@hostname.com:<port>/database-name?options
        _connect_data = __FORMAT__( "mongodb://{}:{}@{}:{}/{}?ssl={}&connectTimeoutMS={}&socketTimeoutMS={}&authMechanism={}",
                                    kfsetting->_user, kfsetting->_password, kfsetting->_ip, kfsetting->_port, kfsetting->_database,
                                    kfsetting->_use_ssl, kfsetting->_connect_timeout, kfsetting->_execute_timeout, kfsetting->_auth_type );

        try
        {
            _is_connected = true;
            _connection.connect( _connect_data, _factory );
        }
        catch ( Poco::Exception& ex )
        {
            _is_connected = false;
            __LOG_ERROR__( "mongo[{}] connect failed = [{}]!", _connect_data, ex.displayText() );
        }
    }

    void KFMongo::ShutDown()
    {
        _is_connected = false;
        _connection.disconnect();
    }

    bool KFMongo::IsConnected()
    {
        return _is_connected;
    }

}