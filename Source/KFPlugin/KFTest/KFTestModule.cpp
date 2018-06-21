#include "KFTestModule.h"
#include "KFTestConfig.h"
#include "KFTestMessage.pb.h"
#include "KFClient/KFClientInterface.h"
#include "KFServer/KFTcpServerInterface.h"
#include "KFNetFunction/KFNetFunctionInterface.h"

namespace KFrame
{
	KFTestModule::KFTestModule()
	{
	}

	KFTestModule::~KFTestModule()
	{
	}

	void KFTestModule::InitModule()
	{
		_kf_config->AddConfig( _kf_test_config, _kf_plugin->_config, true );
	}

	void KFTestModule::BeforeRun()
	{
		_kf_net_function->RegisterFunction( KFMsg::Protocol::MSG_TEST_BUFFER_REQ, this, &KFTestModule::HandleBufferMessage );

		if ( !_kf_test_config->_is_client )
		{
			// 注册链接成功消息
			_kf_tcp_server->RegisterConnectionFunction( "test", this, &KFTestModule::ProcessServerSendTestMessage );
		}
		else
		{
			for ( auto i = 1u; i <= _kf_test_config->_client_count; ++i )
			{
				_kf_client->AddWaitStartClient( i, i, "test", "127.0.0.1", 13101 );
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	void KFTestModule::HandleBufferMessage( uint64 id, const char* data, uint32 length )
	{

	}

	static char buffer[ 100 ] = {};

	void KFTestModule::ProcessClientSendTestMessage( uint64 serverid )
	{
		KFLogger::LogLogic( KFLogger::Info, "Start Send Message!" );

		for ( auto i = 0u; i < _kf_test_config->_message_count; ++i )
		{
			_kf_client->SendNetMessage( KFMsg::Protocol::MSG_TEST_BUFFER_REQ, buffer, sizeof( buffer ) );
		}
	}
	
	void KFTestModule::ProcessServerSendTestMessage( uint64 id, uint32 type, const std::string& ip, uint32 port )
	{
		if ( id == 1000 )
		{
			KFLogger::LogLogic( KFLogger::Info, "Start Send Message!" );

			for ( auto i = 0u; i < _kf_test_config->_message_count; ++i )
			{
				_kf_tcp_server->SendNetMessage( KFMsg::Protocol::MSG_TEST_BUFFER_REQ, buffer, sizeof( buffer ) );
			}
		}
	}
}