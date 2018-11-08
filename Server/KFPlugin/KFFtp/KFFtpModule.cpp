#include "KFFtpModule.h"
#include "KFFtpConfig.h"
#include "KFDownloadThread.h"
#include "KFUploadThread.h"

namespace KFrame
{
    KFFtpModule::KFFtpModule()
    {
        _kf_ftp_thread = nullptr;
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
        if ( _kf_ftp_thread == nullptr )
        {
            return;
        }

        if ( !_kf_ftp_thread->IsFinish() )
        {
            return;
        }

        _kf_ftp_thread->FinishThread();
        _ftp_thread_list.erase( _kf_ftp_thread->_app_path );
        __KF_DESTROY__( KFFtpThread, _kf_ftp_thread );
        _kf_ftp_thread = nullptr;

        if ( _ftp_thread_list.empty() )
        {
            return;
        }

        _kf_ftp_thread = _ftp_thread_list.begin()->second;
        _kf_ftp_thread->StartThread();
    }

    void KFFtpModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_ftp_config );
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
        kfftpthread->_ftp_id = ftpid;
        kfftpthread->_app_path = apppath;
        kfftpthread->_ftp_function = function;

        AddFtpThread( kfftpthread );
    }

    void KFFtpModule::StartFtpUpload( uint32 ftpid, const std::string& apppath, KFFtpFunction& function )
    {
        auto iter = _ftp_thread_list.find( apppath );
        if ( iter != _ftp_thread_list.end() )
        {
            return;
        }

        auto kfftpthread = __KF_CREATE__( KFUploadThread );
        kfftpthread->_ftp_id = ftpid;
        kfftpthread->_app_path = apppath;
        kfftpthread->_ftp_function = function;

        AddFtpThread( kfftpthread );
    }

    void KFFtpModule::AddFtpThread(  KFFtpThread* kfftpthread )
    {
        if ( _kf_ftp_thread == nullptr )
        {
            _kf_ftp_thread = kfftpthread;
            _kf_ftp_thread->StartThread();
        }
        else
        {
            _ftp_thread_list[ kfftpthread->_app_path ] = kfftpthread;
        }
    }
}