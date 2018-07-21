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

    const KFFtpSetting* KFFtpConfig::FindFtpSetting( uint32 id ) const
    {
        return _kf_ftp_setting.Find( id );
    }

    /////////////////////////////////////////////////////////////////////////////
    bool KFFtpConfig::LoadConfig( const char* file )
    {
        _kf_ftp_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto ftpsnode = config.FindNode( "Ftps" );
            while ( ftpsnode.IsValid() )
            {
                auto address = ftpsnode.GetString( "Address" );
                auto port = ftpsnode.GetUInt32( "Port" );
                auto user = ftpsnode.GetString( "User" );
                auto password = ftpsnode.GetString( "Password" );

                auto ftpnode = ftpsnode.FindNode( "Ftp" );
                while ( ftpnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFFtpSetting );

                    kfsetting->_address = address;
                    kfsetting->_port = port;
                    kfsetting->_user = user;
                    kfsetting->_password = password;

                    kfsetting->_id = ftpnode.GetUInt32( "Id" );
                    kfsetting->_win_ftp_path = ftpnode.GetString( "WinFtpPath" );
                    kfsetting->_linux_ftp_path = ftpnode.GetString( "LinuxFtpPath" );
                    kfsetting->_local_path = ftpnode.GetString( "LocalPath" );
                    _kf_ftp_setting.Insert( kfsetting->_id, kfsetting );

                    ftpnode.NextNode();
                }

                ftpsnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}