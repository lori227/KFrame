#include "KFLaunchConfig.h"

namespace KFrame
{
    uint32 KFLaunchSetting::GetShutTime() const
    {
#ifdef __KF_DEBUG__
        return _debug_shut_time;
#else
        return _release_shut_time;
#endif
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFLaunchConfig::KFLaunchConfig()
    {
        _auto_startup = true;
        _launch_redis_id = 1;
        _ftp_id = 1;
    }

    KFLaunchConfig::~KFLaunchConfig()
    {

    }

    KFLaunchSetting* KFLaunchConfig::FindLaunchSetting( uint32 id )
    {
        return _kf_launch_setting.Find( id );
    }

    bool KFLaunchConfig::LoadConfig( const char* file )
    {
        _kf_launch_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Setting" );
            _startup_file = root.GetString( "File" );
            _auto_startup = root.GetBoolen( "AutoStartup" );
            _launch_redis_id = root.GetUInt32( "LaunchRedisId" );
            _ftp_id = root.GetUInt32( "FtpId" );
            /////////////////////////////////////////////////////////////////////

            auto launchnode = root.FindNode( "Launch" );
            while ( launchnode.IsValid() )
            {
                auto kfsetting = __KF_CREATE__( KFLaunchSetting );

                kfsetting->_id = launchnode.GetUInt32( "Id" );
                kfsetting->_startup = launchnode.GetBoolen( "Startup" );
                kfsetting->_show_window = launchnode.GetBoolen( "ShowWindow" );
                kfsetting->_pause = launchnode.GetUInt32( "Pause" );
                kfsetting->_app_id = launchnode.GetUInt32( "AppId" );
                kfsetting->_debug_shut_time = launchnode.GetUInt32( "DebugShutTime" );
                kfsetting->_release_shut_time = launchnode.GetUInt32( "ReleaseShutTime" );
                kfsetting->_app_name = launchnode.GetString( "AppName" );
                kfsetting->_app_path = launchnode.GetString( "Path" );
                kfsetting->_startup_config = launchnode.GetString( "Config" );
                kfsetting->_logger_file = launchnode.GetString( "LoggerFile" );
                kfsetting->_linux_path = launchnode.GetString( "LinuxPath" );
                kfsetting->_linux_restar = launchnode.GetString( "LinuxRestar" );
                _kf_launch_setting.Insert( kfsetting->_id, kfsetting );

                launchnode.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}