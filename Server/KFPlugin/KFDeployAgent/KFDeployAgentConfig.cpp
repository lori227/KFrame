#include "KFDeployAgentConfig.h"

namespace KFrame
{

    std::string KFLaunchSetting::GetStartupFile()
    {
        auto file = _app_file;

#ifdef __KF_DEBUG__
        file += "d";
#endif // __KF_DEBUG__

#if __KF_SYSTEM__ == __KF_WIN__
        file += ".exe";
#endif
        return file;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFDeployAgentConfig::KFDeployAgentConfig()
    {
        _is_auto_startup = true;
        _ftp_id = 1;
    }

    KFDeployAgentConfig::~KFDeployAgentConfig()
    {

    }

    KFLaunchSetting* KFDeployAgentConfig::FindLaunchSetting( uint32 launchid )
    {
        return _kf_launch_setting.Find( launchid );
    }

    bool KFDeployAgentConfig::LoadConfig( const char* file )
    {
        _kf_launch_setting.Clear();

        try
        {
            KFXml kfxml( file );
            auto root = kfxml.FindNode( "Setting" );
            _deploy_path = root.GetString( "DeployPath" );
            _is_auto_startup = root.GetBoolen( "AutoStartup" );
            _ftp_id = root.GetUInt32( "FtpId" );
            /////////////////////////////////////////////////////////////////////

            auto launchs = root.FindNode( "Launchs" );
            while ( launchs.IsValid() )
            {
                auto apppath = launchs.GetString( "AppPath" );
                auto appfile = launchs.GetString( "AppFile" );
                auto appname = launchs.GetString( "AppName" );
                auto apptype = launchs.GetString( "AppType" );
                auto appconfig = launchs.GetString( "AppConfig" );
                auto isshowwindow = launchs.GetBoolen( "ShowWindow" );

                auto launchnode = launchs.FindNode( "Launch" );
                while ( launchnode.IsValid() )
                {
                    auto kfsetting = __KF_CREATE__( KFLaunchSetting );

                    kfsetting->_app_path = apppath;
                    kfsetting->_app_file = appfile;
                    kfsetting->_app_name = appname;
                    kfsetting->_app_type = apptype;
                    kfsetting->_app_config = appconfig;
                    kfsetting->_is_show_window = isshowwindow;

                    kfsetting->_launch_id = launchnode.GetUInt32( "LaunchId" );
                    kfsetting->_app_id = launchnode.GetUInt32( "AppId" );
                    kfsetting->_is_startup = launchnode.GetBoolen( "Startup" );
                    kfsetting->_is_pause = launchnode.GetUInt32( "Pause" );
                    _kf_launch_setting.Insert( kfsetting->_launch_id, kfsetting );

                    launchnode.NextNode();
                }

                launchs.NextNode();
            }
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}