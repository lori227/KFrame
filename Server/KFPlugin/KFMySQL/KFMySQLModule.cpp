#include "KFMySQLModule.h"
#include "KFMySQLConfig.h"
#include "KFThread/KFThread.h"
#include "Poco/Data/MySQL/Connector.h"

namespace KFrame
{
	KFMySQLModule::KFMySQLModule()
	{
	
	}

	KFMySQLModule::~KFMySQLModule()
	{
	
	}

	void KFMySQLModule::InitModule()
	{
		MySQL::Connector::registerConnector();
	
		_kf_config->AddConfig( _kf_mysql_config, _kf_plugin->_plugin_name, _kf_plugin->_config, false );
		///////////////////////////////////////////////////////////////////////////////////////////
	}

	void KFMySQLModule::BeforeShut()
	{
		MySQL::Connector::unregisterConnector();
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
		////////////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////////////
	}

	void KFMySQLModule::ShutDown()
	{

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
		
	KFMySQLDriver* KFMySQLModule::CreateExecute( uint32 id )
	{
		auto kfsetting = _kf_mysql_config->FindSetting( id );
		if ( kfsetting == nullptr )
		{
			return nullptr;
		}

		return CreateExecute( kfsetting->_id, kfsetting->_user, kfsetting->_password, kfsetting->_database, kfsetting->_ip, kfsetting->_port );
	}

	KFMySQLDriver* KFMySQLModule::CreateExecute( const std::string& filed, uint32 logicid /* = 0 */ )
	{
		auto kfsetting = _kf_mysql_config->FindSetting( filed, logicid );
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

		auto kfexecute = __KF_CREATE_BATCH__( KFMySQLExecute, 5 );
		kfexecute->InitMySQL( id, user, password, database, ip, port );

		InsertMySQLExecute( id, kfexecute );
		return kfexecute;
	}
}