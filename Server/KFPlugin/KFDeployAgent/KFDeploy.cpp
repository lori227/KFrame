#include "KFDeploy.h"

namespace KFrame
{
    KFLaunchSetting::KFLaunchSetting()
    {
        _min_id = 0;
        _max_id = 0;
    }

    std::string KFLaunchSetting::GetAppPath()
    {
        return _deploy_path + _app_path + "/";
    }

    std::string KFLaunchSetting::GetStartupFile()
    {
        auto file = _app_file;

#ifdef __KF_DEBUG__
        file += "d";
#endif // __KF_DEBUG__

#if __KF_SYSTEM__ == __KF_WIN__
        file += ".exe";
#endif
        auto apppath = GetAppPath();
        return apppath + file;
    }


    void KFLaunchSetting::CopyFrom( MapString& values )
    {
        _deploy_path = values[ __KF_STRING__( deploypath ) ];
        _ftp_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( ftpid ) ] );
        _app_name = values[ __KF_STRING__( appname ) ];
        _app_type = values[ __KF_STRING__( apptype ) ];
        _app_path = values[ __KF_STRING__( apppath ) ];
        _app_file = values[ __KF_STRING__( appfile ) ];
        _app_config = values[ __KF_STRING__( appconfig ) ];
        _min_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( minid ) ] );
        _max_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( maxid ) ] );
        _log_type = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( logtype ) ] );
        _service = values[ __KF_STRING__( service ) ];
    }

    void KFLaunchSetting::SaveTo( MapString& values )
    {
        values[ __KF_STRING__( deploypath ) ] = _deploy_path;
        values[ __KF_STRING__( ftpid ) ] = __TO_STRING__( _ftp_id );
        values[ __KF_STRING__( appname ) ] = _app_name;
        values[ __KF_STRING__( apptype ) ] = _app_type;
        values[ __KF_STRING__( apppath ) ] = _app_path;
        values[ __KF_STRING__( appfile ) ] = _app_file;
        values[ __KF_STRING__( appconfig ) ] = _app_config;
        values[ __KF_STRING__( minid ) ] = __TO_STRING__( _min_id );
        values[ __KF_STRING__( maxid ) ] = __TO_STRING__( _max_id );
        values[ __KF_STRING__( logtype ) ] = __TO_STRING__( _log_type );
        values[ __KF_STRING__( service ) ] = _service;
    }

    KFDeployData::KFDeployData()
    {
        _app_id = 0;
        _process_id = 0;
        _startup_time = 0;
        _zone_id = 0;
        _agent_id = 0;
        _is_startup = false;
        _is_download = false;
        _kf_launch = nullptr;
    }

    bool KFDeployData::IsAppServer( const std::string& appname, const std::string& apptype, uint32 appid, uint32 zoneid )
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

    void KFDeployData::CopyFrom( MapString& values )
    {
        _app_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( appid ) ] );
        _app_name = values[ __KF_STRING__( appname ) ];
        _app_type = values[ __KF_STRING__( apptype ) ];
        _zone_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( zoneid ) ] );
        _is_startup = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( startup ) ] );
        _process_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( process ) ] );
        _startup_time = KFUtility::ToValue< uint64 >( values[ __KF_STRING__( time ) ] );
        _agent_id = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( agentid ) ] );
        _local_ip = KFUtility::ToValue< uint32 >( values[ __KF_STRING__( localip ) ] );
    }

    void KFDeployData::SaveTo( MapString& values )
    {
        values[ __KF_STRING__( appid ) ] = __TO_STRING__( _app_id );
        values[ __KF_STRING__( appname ) ] = _app_name;
        values[ __KF_STRING__( apptype ) ] = _app_type;
        values[ __KF_STRING__( zoneid ) ] = __TO_STRING__( _zone_id );
        values[ __KF_STRING__( startup ) ] = __TO_STRING__( _is_startup ? 1 : 0 );
        values[ __KF_STRING__( process ) ] = __TO_STRING__( _process_id );
        values[ __KF_STRING__( time ) ] = __TO_STRING__( _startup_time );
        values[ __KF_STRING__( agentid ) ] = __TO_STRING__( _agent_id );
        values[ __KF_STRING__( localip ) ] = _local_ip;
    }

}
