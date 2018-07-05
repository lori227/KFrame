#include "KFMySQL.h"
#include "KFLogger/KFLogger.h"

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
        __KF_DELETE__( _session );
    }

    bool KFMySQL::InitMySQL( uint32 id, const std::string& user, const std::string& password, const std::string& database, const std::string& ip, uint32 port )
    {
        char temp[ 256 ] = { 0 };
        sprintf( temp, "host=%s;port=%u;user=%s;password=%s;db=%s;compress=true;auto-reconnect=true;character-set=utf8",
                 ip.c_str(), port, user.c_str(), password.c_str(), database.c_str() );

        _connect_data = temp;

        try
        {
            _session = __KF_NEW__( Session, SessionFactory::instance().create( MySQL::Connector::KEY, _connect_data ) );
        }
        catch ( Poco::Exception& ex )
        {
            KFLogger::LogInit( KFLogger::Error, "mysql[%u:%s] connect failed = [%s]!",
                               id, _connect_data.c_str(), ex.displayText().c_str() );
            return false;
        }

        auto ok = _session->isConnected();
        if ( ok )
        {
            KFLogger::LogInit( KFLogger::Info, "mysql[%u] connect[ %s|%s:%u ] ok!",
                               id, database.c_str(), ip.c_str(), port );
        }
        else
        {
            KFLogger::LogInit( KFLogger::Error, "mysql[%u] connect[ %s|%s:%u ] failed!",
                               id, database.c_str(), ip.c_str(), port );
        }

        return ok;
    }

    bool KFMySQL::IsConnected()
    {
        return _session->isConnected();
    }

}