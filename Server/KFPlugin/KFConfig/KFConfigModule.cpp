#include "KFConfigModule.h"

namespace KFrame
{
    KFConfigModule::KFConfigModule()
    {
    }

    KFConfigModule::~KFConfigModule()
    {
    }

    void KFConfigModule::ShutDown()
    {
        _kf_config_data.Clear();
    }

    void KFConfigModule::AddConfig( KFConfig* config, const std::string& module, const std::string& file, bool canreload )
    {
        auto kfdata = _kf_config_data.Create( module );

        kfdata->_module = module;
        kfdata->_config = config;
        kfdata->_file = file;
        kfdata->_can_reload = canreload;
    }

    void KFConfigModule::RemoveConfig( const std::string& module )
    {
        _kf_config_data.Remove( module );
    }

    void KFConfigModule::LoadConfig()
    {
        for ( auto& iter : _kf_config_data._objects )
        {
            auto kfdata = iter.second;

            LoadConfig( kfdata->_config, kfdata->_module, kfdata->_file );
        }
    }

    void KFConfigModule::LoadConfig( const std::string& file )
    {
        for ( auto& iter : _kf_config_data._objects )
        {
            auto kfdata = iter.second;
            if ( !kfdata->_can_reload )
            {
                continue;
            }

            auto pos = kfdata->_file.find( file );
            if ( pos == std::string::npos )
            {
                continue;
            }

            LoadConfig( kfdata->_config, kfdata->_module, kfdata->_file );
        }
    }

    void KFConfigModule::LoadConfig( KFConfig* config, const std::string& module, const std::string& file )
    {
        __LOG_DEBUG__( KFLogEnum::Init, "load [{}][{}]!", module, file );

        bool result = config->LoadConfig( file.c_str() );
        if ( result )
        {
            __LOG_DEBUG__( KFLogEnum::Init, "load [{}][{}] ok!", module, file );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Init, "load [{}][{}] failed!", module, file );
        }
    }
}