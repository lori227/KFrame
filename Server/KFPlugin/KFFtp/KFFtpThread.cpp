#include "KFFtpThread.h"
#include "KFFtpConfig.h"
#include "KFTime/KFDate.h"
#include "KFThread/KFThread.h"
#include "KFConvert/KFConvert.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
    #include <direct.h>
    #define __MKDIR__( path ) _mkdir( path.c_str() )
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/wait.h>

    #define __MKDIR__( path ) mkdir( path.c_str(), 0755 )
#endif

namespace KFrame
{
    KFFtpThread::KFFtpThread()
    {
        _ftp_id = 0;
        _ftp_result = KFFtpEnum::Idle;
    }

    void KFFtpThread::StartThread( uint32 id )
    {
        if ( _ftp_result != KFFtpEnum::Idle )
        {
            return;
        }

        _ftp_id = id;
        _ftp_result = KFFtpEnum::Login;
        KFThread::CreateThread( this, &KFFtpThread::Run, __FUNCTION_LINE__ );
    }

    void KFFtpThread::FinishThread()
    {
        _ftp_result = KFFtpEnum::Idle;
    }

    bool KFFtpThread::IsRun()
    {
        return _ftp_result != KFFtpEnum::Idle;
    }

    bool KFFtpThread::IsFinish()
    {
        return _ftp_result == KFFtpEnum::Failed || _ftp_result == KFFtpEnum::Finish;
    }

    uint32 KFFtpThread::FtpResult()
    {
        return _ftp_result;
    }

    void KFFtpThread::Run()
    {
        auto kfsetting = _kf_ftp_config->FindFtpSetting( _ftp_id );
        if ( kfsetting == nullptr )
        {
            _ftp_result = KFFtpEnum::Failed;
            return;
        }


        nsFTP::CFTPClient ftpclient;
        nsFTP::CLogonInfo logonInfo( kfsetting->_address, kfsetting->_port, kfsetting->_user, kfsetting->_password );
        if ( !ftpclient.Login( logonInfo ) )
        {
            _ftp_result = KFFtpEnum::Failed;
            return;
        }

        _ftp_result = KFFtpEnum::Download;

        // 更新文件
        std::string ftppath = "/" + kfsetting->_ftp_path;
        std::string localpath = kfsetting->_local_path;
        DownloadFiles( &ftpclient, ftppath, localpath );

        ftpclient.Logout();
        _ftp_result = KFFtpEnum::Finish;
    }

    void KFFtpThread::CreateLocalDirectory( const std::string& path )
    {
        __MKDIR__( path );
    }

    void KFFtpThread::DeleteLocalFile( const std::string& file )
    {
        remove( file.c_str() );
    }

    void KFFtpThread::RenameFile( const std::string& oldfile, const std::string& newfile )
    {
        rename( oldfile.c_str(), newfile.c_str() );
    }

    bool KFFtpThread::CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return CheckWinFileModifyTime( ftpclient, file, localfile );
#endif

#if __KF_SYSTEM__ == __KF_LINUX__
        return CheckLinuxFileModifyTime( ftpclient, file, localfile );
#endif
    }

    void KFFtpThread::DownloadFiles( nsFTP::CFTPClient* ftpclient, std::string& ftppath, std::string& localpath )
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

    void KFFtpThread::DownloadFile( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& ftpfile, std::string& localfile, std::string& asciifile )
    {
        if ( CheckFileModifyTime( ftpclient, file, localfile ) )
        {
            return;
        }

        std::string downloadfile = localfile + ".new";
        if ( !ftpclient->DownloadFile( ftpfile, downloadfile ) )
        {
            DeleteLocalFile( downloadfile );
            return KFLogger::LogLogic( KFLogger::Error, "download [ %s ] failed!", localfile.c_str() );
        }

        DeleteLocalFile( localfile );
        RenameFile( downloadfile, localfile );

        KFLogger::LogLogic( KFLogger::Info, "download [ %s ] ok!", localfile.c_str() );
    }

#if __KF_SYSTEM__ == __KF_WIN__

    bool KFFtpThread::CheckWinFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile )
    {
        HANDLE hFile = CreateFileA( localfile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if ( hFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        FILETIME ftCreate, ftModify, ftAccess;
        GetFileTime( hFile, &ftCreate, &ftAccess, &ftModify );
        CloseHandle( hFile );

        FILETIME stLocal;
        FileTimeToLocalFileTime( &ftModify, &stLocal );

        SYSTEMTIME systemtime;
        FileTimeToSystemTime( &stLocal, &systemtime );

        KFDate localtime( systemtime.wYear, systemtime.wMonth, systemtime.wDay, systemtime.wHour, systemtime.wMinute, systemtime.wSecond );
        auto _localtime = localtime.GetTime();

        tm _tm;
        gmtime_s( &_tm, &file->MTime() );
        KFDate filetime( _tm.tm_year + TimeEnum::SinceYear, _tm.tm_mon + 1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec );
        auto _ftptime = filetime.GetTime();

        return _localtime >= _ftptime;
    }

#else

    bool KFFtpThread::CheckLinuxFileModifyTime( nsFTP::CFTPClient* ftpclient, nsFTP::TFTPFileStatusShPtr& file, std::string& localfile )
    {
        auto fp = fopen( localfile.c_str(), "r" );
        if ( fp == nullptr )
        {
            return false;
        }

        auto fd = fileno( fp );

        struct stat buf;
        fstat( fd, &buf );
        fclose( fp );
        auto localtime = buf.st_mtime;

        auto _tm = gmtime( &file->MTime() );
        KFDate filetime( _tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec );
        auto ftptime = filetime.GetTime();

        return localtime >= ftptime;
    }

#endif
}