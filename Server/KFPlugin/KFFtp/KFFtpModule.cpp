#include "KFFtpModule.h"
#include "KFFtpConfig.h"

namespace KFrame
{
    KFFtpModule::KFFtpModule()
    {
    }

    KFFtpModule::~KFFtpModule()
    {
        _ftp_thread_list.Clear();
    }

    void KFFtpModule::InitModule()
    {
        /////////////////////////////////////////////////////////////////////////////////
        __KF_ADD_CONFIG__( _kf_ftp_config, true );
    }

    void KFFtpModule::Run()
    {
        std::list< uint32 > removelist;
        for ( auto& iter : _ftp_thread_list._objects )
        {
            auto kfftpthread = iter.second;
            if ( kfftpthread->IsFinish() )
            {
                removelist.push_back( iter.first );
                kfftpthread->FinishThread();
            }
        }

        for ( auto ftpid : removelist )
        {
            _ftp_thread_list.Remove( ftpid );
        }
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
    void KFFtpModule::StartFtpDownload( uint32 ftpid, KFFtpFunction& function )
    {
        auto kfftpthread = _ftp_thread_list.Create( ftpid );
        kfftpthread->StartThread( ftpid, function );
    }
}