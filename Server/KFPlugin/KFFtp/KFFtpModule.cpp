#include "KFFtpModule.h"
#include "KFFtpConfig.h"
#include "KFDownloadThread.h"
#include "KFUploadThread.h"

namespace KFrame
{
    KFFtpModule::KFFtpModule()
    {
    }

    KFFtpModule::~KFFtpModule()
    {
        for ( auto& iter : _ftp_thread_list )
        {
            __KF_DESTROY__( KFFtpThread, iter.second );
        }
        _ftp_thread_list.clear();
    }

    void KFFtpModule::InitModule()
    {
        /////////////////////////////////////////////////////////////////////////////////
        __KF_ADD_CONFIG__( _kf_ftp_config, true );
    }

    void KFFtpModule::Run()
    {
        for ( auto iter = _ftp_thread_list.begin(); iter != _ftp_thread_list.end(); )
        {
            auto kfftpthread = iter->second;
            if ( !kfftpthread->IsFinish() )
            {
                ++iter;
            }
            else
            {
                kfftpthread->FinishThread();
                __KF_DESTROY__( KFFtpThread, kfftpthread );
                iter = _ftp_thread_list.erase( iter );
            }
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
    void KFFtpModule::StartFtpDownload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function )
    {
        auto iter = _ftp_thread_list.find( apppath );
        if ( iter != _ftp_thread_list.end() )
        {
            return;
        }

        auto kfftpthread = __KF_CREATE__( KFDownloadThread );
        kfftpthread->StartThread( ftpid, apppath, function );

        _ftp_thread_list[ apppath ] = kfftpthread;
    }

    void KFFtpModule::StartFtpUpload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function )
    {
        auto iter = _ftp_thread_list.find( apppath );
        if ( iter != _ftp_thread_list.end() )
        {
            return;
        }

        auto kfftpthread = __KF_CREATE__( KFUploadThread );
        kfftpthread->StartThread( ftpid, apppath, function );

        _ftp_thread_list[ apppath ] = kfftpthread;
    }
}