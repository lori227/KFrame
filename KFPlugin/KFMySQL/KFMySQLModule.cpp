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
            iter.second->ShutDown();
        }

        MySQL::Connector::unregisterConnector();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFMySQLConnectOption* KFMySQLModule::FindMySQLConnectOption( const std::string& module, uint32 logic_id )
    {
        auto setting = KFMySQLConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql setting", module, logic_id );
            return nullptr;
        }

        for ( auto& connect_option : setting->_connect_option )
        {
            if ( logic_id >= connect_option._min_logic_id && logic_id <= connect_option._max_logic_id )
            {
                return &connect_option;
            }
        }

        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFMySQLLogic> KFMySQLModule::FindMySQLLogic( uint32 id )
    {
        auto thread_id = KFThread::GetID();
        auto key = MySQLKey( thread_id, id );

        KFLocker lock( _kf_mutex );
        return _mysql_logic_map.Find( key );
    }

    void KFMySQLModule::InsertMySQLLogic( uint32 logic_id, std::shared_ptr<KFMySQLLogic> mysql_logic )
    {
        auto thread_id = KFThread::GetID();
        auto key = MySQLKey( thread_id, logic_id );

        KFLocker lock( _kf_mutex );
        _mysql_logic_map.Insert( key, mysql_logic );
    }

    std::shared_ptr<KFMySQLDriver> KFMySQLModule::Create( const std::string& module, uint32 logic_id /* = 0 */ )
    {
        auto mysql_option = FindMySQLConnectOption( module, logic_id );
        if ( mysql_option == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find mysql option", module, logic_id );
            return nullptr;
        }

        auto mysql_logic = FindMySQLLogic( mysql_option->_runtime_id );
        if ( mysql_logic == nullptr )
        {
            mysql_logic = __MAKE_SHARED__( KFMySQLLogic );
            mysql_logic->Initialize( module, mysql_option );
            InsertMySQLLogic( mysql_option->_runtime_id, mysql_logic );
        }

        return mysql_logic;
    }

}