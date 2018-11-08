#include "KFConfigModule.h"

namespace KFrame
{
    void KFConfigModule::BeforeRun()
    {
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( loadconfig ), &KFConfigModule::LoadConfig );
    }

    void KFConfigModule::ShutDown()
    {
        _kf_config_data.Clear();
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( loadconfig ) );
    }

    __KF_COMMAND_FUNCTION__( KFConfigModule::LoadConfig )
    {
        for ( auto& iter : _kf_config_data._objects )
        {
            auto kfdata = iter.second;
            if ( !kfdata->_can_reload )
            {
                continue;
            }

            auto pos = kfdata->_file.find( param );
            if ( pos == std::string::npos )
            {
                continue;
            }

            LoadConfig( kfdata->_config, kfdata->_file );
        }
    }

    void KFConfigModule::AddConfig( KFConfig* config, const std::string& file, bool canreload )
    {
        auto key = reinterpret_cast< uint64 >( config );
        auto kfdata = _kf_config_data.Create( key );

        kfdata->_config = config;
        kfdata->_file = file;
        kfdata->_can_reload = canreload;
    }

    void KFConfigModule::RemoveConfig( KFConfig* config )
    {
        auto key = reinterpret_cast< uint64 >( config );
        _kf_config_data.Remove( key );
    }

    void KFConfigModule::LoadConfig()
    {
        for ( auto& iter : _kf_config_data._objects )
        {
            auto kfdata = iter.second;

            LoadConfig( kfdata->_config, kfdata->_file );
        }
    }

    void KFConfigModule::LoadConfig( KFConfig* config, const std::string& file )
    {
        __LOG_DEBUG__( "load [{}] start!", file );
        try
        {
            auto ok = config->LoadConfig( file );
            if ( ok )
            {
                __LOG_DEBUG__( "load [{}] ok!", file );
            }
            else
            {
                __LOG_ERROR__( "load [{}] failed!", file );
            }
        }
        catch ( ... )
        {
            __LOG_ERROR__( "load [{}] exception!", file );
        }
    }
}