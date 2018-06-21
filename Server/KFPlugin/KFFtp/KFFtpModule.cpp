#include "KFFtpModule.h"
#include "KFFtpConfig.h"
#include "KFFtpThread.h"

namespace KFrame
{
	KFFtpModule::KFFtpModule()
	{
		_kf_ftp_thread = new KFFtpThread();
	}

	KFFtpModule::~KFFtpModule()
	{
		delete _kf_ftp_thread;
	}

	void KFFtpModule::InitModule()
	{
		/////////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_ftp_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}

	void KFFtpModule::Run()
	{
		if ( !_kf_ftp_thread->IsFinish() )
		{
			return;
		}

		_ftp_function( _kf_ftp_thread->FtpResult() );
		_kf_ftp_thread->FinishThread();
	}

	void KFFtpModule::BeforeRun()
	{
		__REGISTER_RUN_FUNCTION__( &KFFtpModule::Run );
	}

	void KFFtpModule::ShutDown()
	{
		__UNREGISTER_RUN_FUNCTION__();
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	void KFFtpModule::StartDownload( uint32 ftpid, KFFtpFunction& function )
	{
		if ( _kf_ftp_thread->IsRun() )
		{
			return;
		}

		_ftp_function = function;
		_kf_ftp_thread->StartThread( ftpid );
	}
}