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
        _connect_data = KF_FORMAT( "host={};port={};user={};password={};db={};compress=true;auto-reconnect=true;character-set=utf8", ip, port, user, password, database );

        try
        {
            _session = __KF_NEW__( Session, SessionFactory::instance().create( MySQL::Connector::KEY, _connect_data ) );
        }
        catch ( Poco::Exception& ex )
        {
            KF_LOG_ERROR( "mysql[{}:{}] connect failed = [{}]!", id, _connect_data, ex.displayText() );
            return false;
        }

        auto ok = _session->isConnected();
        if ( ok )
        {
            KF_LOG_INFO( "mysql[{}] connect[ {}|{}:{} ] ok!", id, database, ip, port );
        }
        else
        {
            KF_LOG_ERROR( "mysql[{}] connect[ {}|{}:{} ] failed!", id, database, ip, port );
        }

        return ok;
    }

    bool KFMySQL::IsConnected()
    {
        return _session->isConnected();
    }

}