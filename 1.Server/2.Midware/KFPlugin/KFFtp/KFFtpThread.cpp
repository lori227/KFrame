#include "KFFtpThread.h"

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
    KFFtpThread::KFFtpThread()
    {
        _ftp_id = 0;
        _ftp_result = KFFtpEnum::Idle;
        _ftp_function = nullptr;
    }

    KFFtpThread::~KFFtpThread()
    {

    }

    void KFFtpThread::StartThread( uint32 id, const std::string& apppath, KFFtpFunction& function )
    {
        _ftp_id = id;
        _app_path = apppath;
        _ftp_function = function;

        StartThread();
    }

    void KFFtpThread::StartThread()
    {
        if ( _ftp_result != KFFtpEnum::Idle )
        {
            return;
        }

        __LOG_INFO__( "[{}:{}] ftp start!", _app_path, _ftp_id );

        _ftp_result = KFFtpEnum::FtpLogin;
        CreateFtpThread();
    }

    void KFFtpThread::FinishThread()
    {
        if ( _ftp_function != nullptr )
        {
            _ftp_function( _ftp_id, _app_path, _ftp_result == KFFtpEnum::Finish );
        }

        __LOG_INFO__( "[{}:{}] ftp finish!", _app_path, _ftp_id );
    }

    bool KFFtpThread::IsFinish()
    {
        return _ftp_result == KFFtpEnum::Failed || _ftp_result == KFFtpEnum::Finish;
    }

    int64 KFFtpThread::GetLocalFileTime( std::string& localfile )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return GetWinLocalFileTime( localfile );
#else
        return GetLinuxLocalFileTime( localfile );
#endif
    }

#if __KF_SYSTEM__ == __KF_WIN__
    int64 KFFtpThread::GetWinLocalFileTime( std::string& localfile )
    {
        HANDLE hFile = CreateFileA( localfile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if ( hFile == INVALID_HANDLE_VALUE )
        {
            return 0;
        }

        FILETIME ftCreate, ftModify, ftAccess;
        GetFileTime( hFile, &ftCreate, &ftAccess, &ftModify );
        CloseHandle( hFile );

        FILETIME stLocal;
        FileTimeToLocalFileTime( &ftModify, &stLocal );

        SYSTEMTIME systemtime;
        FileTimeToSystemTime( &stLocal, &systemtime );

        KFDate localtime( systemtime.wYear, systemtime.wMonth, systemtime.wDay, systemtime.wHour, systemtime.wMinute, systemtime.wSecond );
        return localtime.GetTime();
    }
#else
    int64 KFFtpThread::GetLinuxLocalFileTime( std::string& localfile )
    {
        auto fp = fopen( localfile.c_str(), "r" );
        if ( fp == nullptr )
        {
            return 0;
        }

        auto fd = fileno( fp );

        struct stat buf;
        fstat( fd, &buf );
        fclose( fp );
        return buf.st_mtime;
    }
#endif
}