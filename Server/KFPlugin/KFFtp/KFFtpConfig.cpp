#include "KFFtpConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFFtpConfig::KFFtpConfig()
    {
    }

    KFFtpConfig::~KFFtpConfig()
    {

    }

    /////////////////////////////////////////////////////////////////////////////
    bool KFFtpConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto ftpnode = config.FindNode( "Ftp" );
            while ( ftpnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFFtpSetting );

                kfsetting->_id = ftpnode.GetUInt32( "Id" );
                kfsetting->_address = ftpnode.GetString( "Address" );
                kfsetting->_port = ftpnode.GetUInt32( "Port" );
                kfsetting->_user = ftpnode.GetString( "User" );
                kfsetting->_password = ftpnode.GetString( "Password" );
                kfsetting->_ftp_root_path = ftpnode.GetString( "FtpRootPath" );
                kfsetting->_download_path = ftpnode.GetString( "DownLoadPath" );
                kfsetting->_upload_path = ftpnode.GetString( "UpLoadPath" );
                kfsetting->_time_difference = ftpnode.GetUInt32( "TimeDifference" );
                _ftp_setting.Insert( kfsetting->_id, kfsetting );

                ftpnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }

    const KFFtpSetting* KFFtpConfig::FindFtpSetting( uint32 ftpid )
    {
        return _ftp_setting.Find( ftpid );
    }

    std::string KFFtpSetting::GetFtpPath( const std::string& apppath ) const
    {
        char buff[ 128 ] = "";

#if __KF_SYSTEM__ == __KF_WIN__
        sprintf( buff, "%s/%s/%s", _ftp_root_path.c_str(), apppath.c_str(), __KF_CHAR__( win64 ) );
#else
        sprintf( buff, "%s/%s/%s", _ftp_root_path.c_str(), apppath.c_str(), __KF_CHAR__( linux ) );
#endif
        return buff;
    }

    std::string KFFtpSetting::GetDownloadPath( const std::string& apppath ) const
    {
        char buff[ 128 ] = "";

#if __KF_SYSTEM__ == __KF_WIN__
        sprintf( buff, "%s/%s", _download_path.c_str(), apppath.c_str() );
#else
        sprintf( buff, "%s/%s", _download_path.c_str(), apppath.c_str() );
#endif
        return buff;
    }

    std::string KFFtpSetting::GetUploadPath( const std::string& apppath ) const
    {
        char buff[ 128 ] = "";

#if __KF_SYSTEM__ == __KF_WIN__
        sprintf( buff, "%s/%s/%s", _upload_path.c_str(), apppath.c_str(), __KF_CHAR__( win64 ) );
#else
        sprintf( buff, "%s/%s/%s", _upload_path.c_str(), apppath.c_str(), __KF_CHAR__( linux ) );
#endif
        return buff;
    }
}