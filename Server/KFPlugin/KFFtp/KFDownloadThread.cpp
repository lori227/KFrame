#include "KFDownloadThread.h"
#include "KFFtpConfig.h"
#include "KFTime/KFDate.h"
#include "KFThread/KFThread.h"
#include "KFConvert/KFConvert.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
    #include <direct.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
#endif

namespace KFrame
{
    void KFDownloadThread::CreateFtpThread()
    {
        KFThread::CreateThread( this, &KFDownloadThread::Run, __FUNCTION_LINE__ );
    }

    void KFDownloadThread::Run()
    {
        auto kfsetting = _kf_ftp_config->FindFtpSetting( _ftp_id );
        if ( kfsetting == nullptr )
        {
            _ftp_result = KFFtpEnum::Failed;
            return;
        }

        _time_difference = kfsetting->_time_difference;
        try
        {
            nsFTP::CFTPClient ftpclient;
            nsFTP::CLogonInfo logonInfo( kfsetting->_address, kfsetting->_port, kfsetting->_user, kfsetting->_password );
            if ( !ftpclient.Login( logonInfo ) )
            {
                _ftp_result = KFFtpEnum::Failed;
                return;
            }

            _ftp_result = KFFtpEnum::Process;
            //ftpclient.SetResumeMode( false );

            // 更新文件
            std::string ftppath = kfsetting->GetFtpPath( _app_path );
            std::string localpath = kfsetting->GetDownloadPath( _app_path );
            DownloadFiles( &ftpclient, ftppath, localpath );

            ftpclient.Logout();
            _ftp_result = KFFtpEnum::Finish;
        }
        catch ( ... )
        {
        }

    }

    void KFDownloadThread::CreateLocalDirectory( const std::string& path )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _mkdir( path.c_str() );
#else
        mkdir( path.c_str(), 0755 );
#endif
    }

    void KFDownloadThread::DeleteLocalFile( const std::string& file )
    {
        remove( file.c_str() );
    }

    void KFDownloadThread::RenameFile( const std::string& oldfile, const std::string& newfile )
    {
        rename( oldfile.c_str(), newfile.c_str() );
#if __KF_SYSTEM__ == __KF_LINUX__
        chmod( newfile.c_str(), 0777 );
#endif
    }

    bool KFDownloadThread::CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile )
    {
        // local
        auto localtime = GetLocalFileTime( localfile );

        // ftp
        auto _tm = gmtime( &file->MTime() );
        KFDate filetime( _tm->tm_year + KFTimeEnum::SinceYear, _tm->tm_mon + 1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec );

        auto ftptime = static_cast< int64 >( filetime.GetTime() );
        return localtime <= ftptime;
    }

    void KFDownloadThread::DownloadFiles( nsFTP::CFTPClient* ftpclient, std::string& ftppath, std::string& localpath )
    {
        // 创建文件夹
        CreateLocalDirectory( localpath );

        // 遍历ftp上的文件
        nsFTP::TFTPFileStatusShPtrVec filelist;
        ftpclient->List( ftppath, filelist, true );
        for ( auto iter = filelist.begin(); iter != filelist.end(); ++iter )
        {
            auto file = ( *iter );

            auto asciifile = KFConvert::ToAscii( file->Name() );

            std::string childftppath = ftppath + "/" + file->Name();
            std::string childlocalpath = localpath + "/" + asciifile;

            if ( file->IsCwdPossible() )
            {
                DownloadFiles( ftpclient, childftppath, childlocalpath );
            }
            else
            {
                DownloadFile( ftpclient, file, childftppath, childlocalpath, asciifile );
            }
        }
    }

    void KFDownloadThread::DownloadFile( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& ftpfile, std::string& localfile, std::string& asciifile )
    {
        if ( !CheckFileModifyTime( ftpclient, file, localfile ) )
        {
            return;
        }

        std::string downloadfile = localfile + ".new";
        if ( !ftpclient->DownloadFile( ftpfile, downloadfile ) )
        {
            DeleteLocalFile( downloadfile );
            return KFLogger::LogSystem( KFLogger::Error, "download [ %s ] failed!", localfile.c_str() );
        }

        DeleteLocalFile( localfile );
        RenameFile( downloadfile, localfile );

        KFLogger::LogSystem( KFLogger::Info, "download [ %s ] ok!", localfile.c_str() );
    }
}