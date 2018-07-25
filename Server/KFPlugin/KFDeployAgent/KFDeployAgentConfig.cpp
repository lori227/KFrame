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

    bool KFLaunchSetting::IsAppServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid )
    {
        if ( appname == _globbing_str )
        {
            return true;
        }

        if ( appname != _app_name )
        {
            return false;
        }

        if ( zoneid != _zone_id )
        {
            return false;
        }

        if ( apptype == _globbing_str )
        {
            return true;
        }

        if ( apptype != _app_type )
        {
            return false;
        }

        if ( appid == _invalid_int )
        {
            return true;
        }

        if ( appid != _app_id )
        {
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFDeployAgentConfig::KFDeployAgentConfig()
    {
        _is_auto_startup = true;
        _ftp_id = 1;
        _is_show_window = true;
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
            _ftp_id = root.GetUInt32( "FtpId" );
            _is_auto_startup = root.GetBoolen( "AutoStartup" );
            _is_show_window = root.GetBoolen( "ShowWindow" );
            /////////////////////////////////////////////////////////////////////

            auto launchs = root.FindNode( "Launchs" );
            while ( launchs.IsValid() )
            {
                auto apppath = launchs.GetString( "AppPath" );
                auto appfile = launchs.GetString( "AppFile" );
                auto appname = launchs.GetString( "AppName" );
                auto ftpid = launchs.GetUInt32( "FtpId" );
                auto zoneid = launchs.GetUInt32( "ZoneId", true, _invalid_int );

                auto launchnode = launchs.FindNode( "Launch" );
                while ( launchnode.IsValid() )
                {
                    auto apptype = launchnode.GetString( "AppType" );
                    auto appid = launchnode.GetUInt32( "AppId" );
                    auto appcount = launchnode.GetUInt32( "AppCount" );
                    auto appconfig = launchnode.GetString( "AppConfig" );
                    if ( zoneid != _invalid_int )
                    {
                        appid = KFUtility::CalcZoneServerId( appid, zoneid );
                    }

                    for ( auto i = 1u; i <= appcount; ++i )
                    {
                        auto kfsetting = __KF_CREATE__( KFLaunchSetting );
                        kfsetting->_app_path = apppath;
                        kfsetting->_app_file = appfile;
                        kfsetting->_app_name = appname;
                        kfsetting->_app_type = apptype;
                        kfsetting->_app_id = appid + i - 1;
                        kfsetting->_zone_id = zoneid;
                        kfsetting->_app_config = appconfig;
                        _kf_launch_setting.Insert( kfsetting->_app_id, kfsetting );
                    }

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