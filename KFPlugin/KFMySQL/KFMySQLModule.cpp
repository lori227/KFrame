#include "KFMySQLModule.hpp"
#include "Poco/Data/MySQL/Connector.h"

namespace KFrame
{
    void KFMySQLModule::BeforeRun()
    {
        MySQL::Connector::registerConnector();
    }

    void KFMySQLModule::ShutDown()
    {
        KFLocker lock( _kf_mutex );
        for ( auto& iter : _mysql_logic_map._objects )
        {
            auto kflogic = iter.second;
            kflogic->ShutDown();
        }

        MySQL::Connector::unregisterConnector();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMySQLLogic* KFMySQLModule::FindMySQLLogic( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MySQLKey( threadid, id );

        KFLocker lock( _kf_mutex );
        return _mysql_logic_map.Find( key );
    }

    void KFMySQLModule::InsertMySQLLogic( uint32 id, KFMySQLLogic* kflogic )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MySQLKey( threadid, id );

        KFLocker lock( _kf_mutex );
        _mysql_logic_map.Insert( key, kflogic );
    }

    KFMySQLDriver* KFMySQLModule::Create( const std::string& module, uint32 logicid /* = 0 */ )
    {
        auto kfmysqltype = KFMySQLConfig::Instance()->FindMySQLType( module, logicid );
        if ( kfmysqltype == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql type", module, logicid );
            return nullptr;
        }

        auto kflogic = FindMySQLLogic( kfmysqltype->_id );
        if ( kflogic != nullptr )
        {
            return kflogic;
        }

        kflogic = __KF_NEW__( KFMySQLLogic );
        kflogic->Initialize( kfmysqltype );
        InsertMySQLLogic( kfmysqltype->_id, kflogic );

        return kflogic;
    }

}