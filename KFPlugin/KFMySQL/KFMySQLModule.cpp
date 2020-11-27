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
    const KFMySQLConnectOption* KFMySQLModule::FindMySQLConnectOption( const std::string& module, uint32 logicid )
    {
        auto kfsetting = KFMySQLConfig::Instance()->FindSetting( module );
        if ( kfsetting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql setting", module, logicid );
            return nullptr;
        }

        for ( auto& connectoption : kfsetting->_connect_option )
        {
            if ( logicid >= connectoption._min_logic_id && logicid <= connectoption._max_logic_id )
            {
                return &connectoption;
            }
        }

        return nullptr;
    }
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
        auto kfmysqloption = FindMySQLConnectOption( module, logicid );
        if ( kfmysqloption == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql option", module, logicid );
            return nullptr;
        }

        auto kflogic = FindMySQLLogic( kfmysqloption->_runtime_id );
        if ( kflogic != nullptr )
        {
            return kflogic;
        }

        kflogic = __KF_NEW__( KFMySQLLogic );
        kflogic->Initialize( module, kfmysqloption );
        InsertMySQLLogic( kfmysqloption->_runtime_id, kflogic );

        return kflogic;
    }

}