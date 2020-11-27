#include "KFMySQL.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{

    KFMySQL::KFMySQL()
    {
        _session = nullptr;
    }

    KFMySQL::~KFMySQL()
    {
        __DELETE_OBJECT__( _session );
    }

    void KFMySQL::InitMySQL( const std::string& database, const std::string& user, const std::string& password, const std::string& ip, uint32 port )
    {
        _connect_data = __FORMAT__( "host={};port={};user={};password={};db={};compress=true;auto-reconnect=true;character-set=utf8",
                                    ip, port, user, password, database );

        try
        {
            _session = __NEW_OBJECT__( Session, SessionFactory::instance().create( MySQL::Connector::KEY, _connect_data ) );
            auto ok = _session->isConnected();
            if ( ok )
            {
                __LOG_INFO__( "mysql connect[ {}|{}:{} ] ok", database, ip, port );
            }
            else
            {
                __LOG_ERROR__( "mysql connect[ {}|{}:{} ] failed", database, ip, port );
            }
        }
        catch ( Poco::Exception& ex )
        {
            __LOG_ERROR__( "mysql[{}] connect failed = [{}]", _connect_data, ex.displayText() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "mysql[{}] connect failed = unknown", _connect_data );
        }
    }

    void KFMySQL::ShutDown()
    {
        _session->close();
    }

    bool KFMySQL::IsConnected()
    {
        return _session->isConnected();
    }

}