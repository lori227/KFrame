#include "KFMySQLModule.h"
#include "KFMySQLConfig.h"
#include "Poco/Data/MySQL/Connector.h"

namespace KFrame
{
    void KFMySQLModule::InitModule()
    {
        MySQL::Connector::registerConnector();
        __KF_ADD_CONFIG__( _kf_mysql_config, false );
    }

    void KFMySQLModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_mysql_config );
        MySQL::Connector::unregisterConnector();
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFMySQLDriver* KFMySQLModule::FindMySQLExecute( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MySQLKey( threadid, id );

        KFLocker lock( _kf_mutex );
        return _mysql_execute_map.Find( key );
    }

    void KFMySQLModule::InsertMySQLExecute( uint32 id, KFMySQLExecute* kfexecute )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = MySQLKey( threadid, id );

        KFLocker lock( _kf_mutex );
        _mysql_execute_map.Insert( key, kfexecute );
    }

    KFMySQLDriver* KFMySQLModule::CreateExecute( const std::string& module, uint32 logicid /* = 0 */ )
    {
        auto kfsetting = _kf_mysql_config->FindSetting( module, logicid );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return CreateExecute( kfsetting->_id, kfsetting->_user, kfsetting->_password, kfsetting->_database, kfsetting->_ip, kfsetting->_port );
    }

    KFMySQLDriver* KFMySQLModule::CreateExecute( uint32 id, const std::string& user, const std::string& password, const std::string& database, const std::string& ip, uint32 port )
    {
        auto kfdriver = FindMySQLExecute( id );
        if ( kfdriver != nullptr )
        {
            return kfdriver;
        }

        auto kfexecute = __KF_NEW__( KFMySQLExecute );
        kfexecute->InitMySQL( id, user, password, database, ip, port );

        InsertMySQLExecute( id, kfexecute );
        return kfexecute;
    }
}