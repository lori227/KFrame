#include "KFConfigModule.hpp"

namespace KFrame
{
    void KFConfigModule::ShutDown()
    {
        _kf_config_data.Clear();
    }

    bool KFConfigModule::AddConfig( KFConfig* config, const std::string& file, bool canreload )
    {
        if ( file.empty() && config->_file.empty() )
        {
            return false;
        }

        auto key = reinterpret_cast< uint64 >( config );
        auto kfdata = _kf_config_data.Create( key );

        kfdata->_config = config;
        kfdata->_can_reload = canreload;

        if ( !file.empty() )
        {
            kfdata->_file = file;
        }
        else
        {
            static std::string _path = "./config/";
            kfdata->_file = _path + config->_file;
        }

        // 加载配置文件
        LoadConfig( config, kfdata->_file );
        return true;
    }

    void KFConfigModule::RemoveConfig( KFConfig* config )
    {
        auto key = reinterpret_cast< uint64 >( config );
        _kf_config_data.Remove( key );
    }

    void KFConfigModule::LoadConfig( KFConfig* config, const std::string& file )
    {
        __LOG_INFO__( "load [{}] start!", file );
        try
        {
            auto ok = config->LoadConfig( file );
            if ( ok )
            {
                __LOG_INFO__( "load [{}] ok!", file );
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


    void KFConfigModule::ReloadConfig( const std::string& file )
    {
        for ( auto& iter : _kf_config_data._objects )
        {
            auto kfdata = iter.second;
            if ( !kfdata->_can_reload )
            {
                continue;
            }

            if ( file != _globbing_str )
            {
                auto pos = kfdata->_file.find( file );
                if ( pos == std::string::npos )
                {
                    continue;
                }
            }

            LoadConfig( kfdata->_config, kfdata->_file );
        }
    }

}