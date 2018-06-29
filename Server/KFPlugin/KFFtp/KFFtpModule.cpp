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
        __KF_ADD_CONFIG__( _kf_ftp_config, true );
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
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();
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