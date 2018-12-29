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
    bool KFFtpConfig::LoadConfig()
    {
        _ftp_setting.Clear();
        auto kfglobal = KFGlobal::Instance();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto ftpsnode = config.FindNode( "Ftps" );
        auto ftpnode = ftpsnode.FindNode( "Ftp" );
        while ( ftpnode.IsValid() )
        {
            auto kfsetting = __KF_NEW__( KFFtpSetting );

            kfsetting->_id = ftpnode.GetUInt32( "FtpId" );
            kfsetting->_address = ftpnode.GetString( "Address" );
            kfsetting->_port = ftpnode.GetUInt32( "Port" );
            kfsetting->_user = ftpnode.GetString( "User" );
            kfsetting->_password = ftpnode.GetString( "Password" );
            kfsetting->_ftp_root_path = ftpnode.GetString( "FtpRootPath" );
            kfsetting->_download_path = ftpnode.GetString( "DownLoadPath" );
            kfsetting->_upload_path = ftpnode.GetString( "UpLoadPath" );
            kfsetting->_time_difference = ftpnode.GetUInt32( "TimeDifference" );
            auto channelnode = ftpnode.FindNode( "Channel" );
            while ( channelnode.IsValid() )
            {
                auto channelid = channelnode.GetUInt32( "ChannelId" );
                if ( kfglobal->CheckChannelService( channelid, _invalid_int ) )
                {
                    kfsetting->_address = channelnode.GetString( "Address" );
                    kfsetting->_port = channelnode.GetUInt32( "Port" );
                    kfsetting->_user = channelnode.GetString( "User" );
                    kfsetting->_password = channelnode.GetString( "Password" );
                    break;
                }

                channelnode.NextNode();
            }

            _ftp_setting.Insert( kfsetting->_id, kfsetting );
            ftpnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }

    const KFFtpSetting* KFFtpConfig::FindFtpSetting( uint32 ftpid )
    {
        return _ftp_setting.Find( ftpid );
    }

    std::string KFFtpSetting::GetFtpPath( const std::string& apppath ) const
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return __FORMAT__( "{}/{}/{}", _ftp_root_path, apppath, __KF_STRING__( win64 ) );
#else
        return __FORMAT__( "{}/{}/{}", _ftp_root_path, apppath, __KF_STRING__( linux ) );
#endif
    }

    std::string KFFtpSetting::GetDownloadPath( const std::string& apppath ) const
    {
        return __FORMAT__( "{}/{}", _download_path, apppath );
    }

    std::string KFFtpSetting::GetUploadPath( const std::string& apppath ) const
    {
#if __KF_SYSTEM__ == __KF_WIN__
        return __FORMAT__( "{}/{}/{}", _upload_path, apppath, __KF_STRING__( win64 ) );
#else
        return __FORMAT__( "{}/{}/{}", _upload_path, apppath, __KF_STRING__( linux ) );
#endif
    }
}