#include "KFDeploy.hpp"

namespace KFrame
{
    KFDeployData::KFDeployData()
    {
        _process_id = 0;
        _startup_time = 0;
        _zone_id = 0;
        _is_startup = false;
        _is_shutdown = false;
        _is_debug = false;
    }

    std::string KFDeployData::GetAppPath()
    {
        auto app_path = _invalid_string;
        if ( _app_path.empty() )
        {
            app_path = _deploy_path + "/";
        }
        else
        {
            app_path = _deploy_path + "/" + _app_path + "/";
        }

        return app_path;
    }

    std::string KFDeployData::GetStartupFile( bool is_debug )
    {
        auto file = _app_file;
        if ( is_debug )
        {
            file += "d";
        }

#if __KF_SYSTEM__ == __KF_WIN__
        file += ".exe";
#endif
        auto app_path = GetAppPath();
        return app_path + file;
    }

    bool KFDeployData::IsAppServer( const std::string& app_name, const std::string& app_type, const std::string& app_id, uint32 zone_id )
    {
        // 指定appid
        if ( app_id != _globbing_string )
        {
            return ( app_id == _app_id );
        }

        // appname
        if ( app_name != _globbing_string )
        {
            if ( app_name != _app_name )
            {
                return false;
            }
        }

        // apptype
        if ( app_type != _globbing_string )
        {
            if ( app_type != _app_type )
            {
                return false;
            }
        }

        // zone_id
        if ( app_name == __STRING__( zone ) )
        {
            if ( zone_id != _invalid_int )
            {
                if ( zone_id != _zone_id )
                {
                    return false;
                }
            }
        }

        return true;
    }

    void KFDeployData::CopyFrom( StringMap& values )
    {
        _deploy_path = values[ __STRING__( deploypath ) ];
        _app_path = values[ __STRING__( apppath ) ];
        _app_file = values[ __STRING__( appfile ) ];

        _app_id = values[ __STRING__( appid ) ];
        KFAppId app_id( _app_id );
        _zone_id = app_id.GetZoneId();

        _app_name = values[ __STRING__( appname ) ];
        _app_type = values[ __STRING__( apptype ) ];

        _is_startup = __TO_UINT32__( values[ __STRING__( startup ) ] );
        _is_debug = __TO_UINT32__( values[ __STRING__( debug ) ] );
        _is_shutdown = __TO_UINT32__( values[ __STRING__( shutdown ) ] );
        _process_id = __TO_UINT32__( values[ __STRING__( process ) ] );
        _startup_time = __TO_UINT64__( values[ __STRING__( time ) ] );
        _agent_id = values[ __STRING__( agentid ) ];
        _local_ip = values[ __STRING__( localip ) ];
        _service_type = values[ __STRING__( service ) ];
        _net_type = values[ __STRING__( net ) ];
        _params = values[ __STRING__( param ) ];
        _heartbeat = __TO_UINT32__( values[ __STRING__( heartbeat ) ] );
    }

    void KFDeployData::SaveTo( StringMap& values )
    {
        values[ __STRING__( deploypath ) ] = _deploy_path;
        values[ __STRING__( apppath ) ] = _app_path;
        values[ __STRING__( appfile ) ] = _app_file;

        values[ __STRING__( appid ) ] = _app_id;
        values[ __STRING__( appname ) ] = _app_name;
        values[ __STRING__( apptype ) ] = _app_type;
        values[ __STRING__( shutdown ) ] = __TO_STRING__( _is_shutdown ? 1 : 0 );
        values[ __STRING__( startup ) ] = __TO_STRING__( _is_startup ? 1 : 0 );
        values[ __STRING__( debug ) ] = __TO_STRING__( _is_startup ? 1 : 0 );
        values[ __STRING__( process ) ] = __TO_STRING__( _process_id );
        values[ __STRING__( time ) ] = __TO_STRING__( _startup_time );
        values[ __STRING__( agentid ) ] = _agent_id;
        values[ __STRING__( localip ) ] = _local_ip;
        values[ __STRING__( service ) ] = _service_type;
        values[ __STRING__( net ) ] = _net_type;
        values[ __STRING__( param ) ] = _params;
        values[ __STRING__( heartbeat ) ] = __TO_STRING__( _heartbeat );
    }

}
