#include "KFMySQL.hpp"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    using namespace Poco::Data;

    KFMySQL::KFMySQL()
    {
        _session = nullptr;
    }

    KFMySQL::~KFMySQL()
    {
        delete _session;
    }

    bool KFMySQL::InitMySQL( const std::string& user, const std::string& password, const std::string& database, const std::string& ip, uint32 port )
    {
        _connect_data = __FORMAT__( "host={};port={};user={};password={};db={};compress=true;auto-reconnect=true;character-set=utf8",
                                    ip, port, user, password, database );

        try
        {
            _session = new Session( SessionFactory::instance().create( MySQL::Connector::KEY, _connect_data ) );
            auto ok = _session->isConnected();
            if ( ok )
            {
                __LOG_INFO__( "mysql connect[ {}|{}:{} ] ok!", database, ip, port );
            }
            else
            {
                __LOG_ERROR__( "mysql connect[ {}|{}:{} ] failed!", database, ip, port );
            }

            return ok;
        }
        catch ( Poco::Exception& ex )
        {
            __LOG_ERROR__( "mysql[{}] connect failed = [{}]!", _connect_data, ex.displayText() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "mysql[{}] connect failed = unknown!", _connect_data );
        }

        return false;
    }

    bool KFMySQL::IsConnected()
    {
        return _session->isConnected();
    }

}