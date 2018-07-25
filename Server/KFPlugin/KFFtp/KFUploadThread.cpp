#include "KFUploadThread.h"
#include "KFFtpConfig.h"
#include "KFTime/KFDate.h"
#include "KFThread/KFThread.h"
#include "KFConvert/KFConvert.h"

#if __KF_SYSTEM__ == __KF_WIN__
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
#else
    #include <unistd.h>
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
#endif

namespace KFrame
{
    void KFUploadThread::CreateFtpThread()
    {
        KFThread::CreateThread( this, &KFUploadThread::Run, __FUNCTION_LINE__ );
    }

    void KFUploadThread::Run()
    {
        auto kfsetting = _kf_ftp_config->FindFtpSetting( _ftp_id );
        if ( kfsetting == nullptr )
        {
            _ftp_result = KFFtpEnum::Failed;
            return;
        }

        _time_difference = kfsetting->_time_difference;

        std::string localpath = kfsetting->GetUploadPath( _app_path );
        auto uploadpath = __NEW_OBJECT__( KFUpLoadPath );
        uploadpath->_total_path = localpath;
        ListAllLocalFiles( uploadpath );

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

            // 更新文件
            std::string ftppath = kfsetting->GetFtpPath( _app_path );
            UploadFiles( &ftpclient, uploadpath, ftppath );

            ftpclient.Logout();
            _ftp_result = KFFtpEnum::Finish;
        }
        catch ( ... )
        {
        }

        __DELETE_OBJECT__( uploadpath );
    }

    void KFUploadThread::ListAllLocalFiles( KFUpLoadPath* uploadpath )
    {
#if __KF_SYSTEM__ == __KF_WIN__
        _finddata_t finddata;

        auto findfile = uploadpath->_total_path + "/*.*";
        auto handle = _findfirst( findfile.c_str(), &finddata );
        if ( handle == -1 )
        {
            return;
        }

        do
        {
            if ( finddata.attrib & _A_SUBDIR )
            {
                if ( strcmp( finddata.name, "." ) == 0 ||
                        strcmp( finddata.name, ".." ) == 0 )
                {
                    continue;
                }

                auto subupdatapath = __NEW_OBJECT__( KFUpLoadPath );
                subupdatapath->_path_name = finddata.name;
                subupdatapath->_total_path = uploadpath->_total_path + "/" + finddata.name;
                uploadpath->_sub_paths.push_back( subupdatapath );

                ListAllLocalFiles( subupdatapath );
            }
            else
            {
                uploadpath->_files.push_back( finddata.name );
            }

        } while ( _findnext( handle, &finddata ) == 0 );

        _findclose( handle );
#else
        auto dir = opendir( uploadpath->_total_path.c_str() );
        if ( dir == nullptr )
        {
            return;
        }

        struct dirent* finddata;
        while ( ( finddata = readdir( dir ) ) != nullptr )
        {
            if ( strcmp( finddata->d_name, "." ) == 0 ||
                    strcmp( finddata->d_name, ".." ) == 0 )
            {
                continue;
            }

            if ( finddata->d_type == 8 )
            {
                uploadpath->_files.push_back( finddata->d_name );
            }
            else if ( finddata->d_type == 4 )
            {
                auto subupdatapath = __NEW_OBJECT__( KFUpLoadPath );
                subupdatapath->_path_name = finddata->d_name;
                subupdatapath->_total_path = uploadpath->_total_path + "/" + finddata->d_name;
                uploadpath->_sub_paths.push_back( subupdatapath );

                ListAllLocalFiles( subupdatapath );
            }
        }

        closedir( dir );
#endif
    }

    void KFUploadThread::UploadFiles( nsFTP::CFTPClient* ftpclient, KFUpLoadPath* uploadpath, std::string& ftppath )
    {
        // 创建目录
        auto newftppath = ftppath;
        if ( !uploadpath->_path_name.empty() )
        {
            newftppath = ftppath + "/" + uploadpath->_path_name;
        }

        ftpclient->MakeDirectory( newftppath.c_str() );

        // 先上传文件
        for ( auto& filename : uploadpath->_files )
        {
            auto localfile = uploadpath->_total_path + "/" + filename;
            auto ftpfile = newftppath + "/" + filename;

            UploadFile( ftpclient, localfile, ftpfile );
        }

        // 子文件夹
        for ( auto subuploadpath : uploadpath->_sub_paths )
        {
            UploadFiles( ftpclient, subuploadpath, newftppath );
        }
    }

    void KFUploadThread::UploadFile( nsFTP::CFTPClient* ftpclient, std::string& localfile, std::string& ftpfile )
    {
        // 判断文件时间
        if ( !CheckFileModifyTime( ftpclient, localfile, ftpfile ) )
        {
            return;
        }

        // 先删除, 存在更新会提示权限错误( 提示覆盖 )
        ftpclient->Delete( ftpfile );

        // 上传文件
        auto ok = ftpclient->UploadFile( localfile, ftpfile );
        if ( ok )
        {
            KFLogger::LogSystem( KFLogger::Info, "upload [ %s ] ok!", localfile.c_str() );
        }
        else
        {
            KFLogger::LogSystem( KFLogger::Error, "upload [ %s ] failed!", localfile.c_str() );
        }
    }

    bool KFUploadThread::CheckFileModifyTime( nsFTP::CFTPClient* ftpclient, std::string& localfile, std::string& ftpfile )
    {
        // local
        auto localtime = GetLocalFileTime( localfile );

        // ftp
        tm _tm;
        ftpclient->FileModificationTime( ftpfile, _tm );

        KFDate kfdate( _tm.tm_year, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec );
        auto ftptime = static_cast< int64 >( kfdate.GetTime() );
        ftptime += _time_difference;

        return localtime >= ftptime;
    }
}