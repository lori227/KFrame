#include "KFLoaderModule.hpp"

namespace KFrame
{
    static std::string _config_path = "./config/";
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    KFLoaderModule::KFLoaderModule()
    {
        _kf_loader_config = __NEW_OBJECT__( KFLoaderConfig );
        _kf_version_config = __NEW_OBJECT__( KFVersionConfig );
    }

    KFLoaderModule::~KFLoaderModule()
    {
        __DELETE_OBJECT__( _kf_loader_config );
        __DELETE_OBJECT__( _kf_version_config );
    }

    void KFLoaderModule::BeforeRun()
    {
        _kf_plugin_manage->RegisterCommandFunction( __STRING__( reloadconfig ), this, &KFLoaderModule::ProcessReloadCommand );
    }

    void KFLoaderModule::ShutDown()
    {
        _kf_plugin_manage->UnRegisterCommandFunction( __STRING__( reloadconfig ) );

        for ( auto& iter : _config_list )
        {
            __DELETE_OBJECT__( iter.second );
        }
        _config_list.clear();
    }

    void KFLoaderModule::ProcessReloadCommand( const StringVector& params )
    {
        ReloadConfig();
    }

    void KFLoaderModule::AddConfig( const std::string& name, KFConfig* config )
    {
        _config_list[ name ] = config;
    }

    KFConfig* KFLoaderModule::FindConfig( const std::string& name )
    {
        auto iter = _config_list.find( name );
        if ( iter == _config_list.end() )
        {
            return nullptr;
        }

        return iter->second;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////
    void KFLoaderModule::LoadConfigListAndVersion()
    {
        {
            auto config_file_path = _config_path + "frameconfig.xml";
            LoadConfigFile( _kf_loader_config, config_file_path, KFLoaderEnum::ClearTypeAll );
        }
        {
            auto config_file_path = _config_path + "config.xml";
            LoadConfigFile( _kf_loader_config, config_file_path, KFLoaderEnum::ClearTypeNone );
        }

        {
            auto config_file_path = _config_path + "version.xml";
            LoadConfigFile( _kf_version_config, config_file_path, KFLoaderEnum::ClearTypeAll );
        }
    }

    void KFLoaderModule::LoadConfig()
    {
        // 读取配置文件列表
        LoadConfigListAndVersion();

        for ( auto& iter : _config_list )
        {
            auto config = iter.second;
            if ( config->_file_name.empty() )
            {
                continue;
            }

            auto loader_setting = _kf_loader_config->FindSetting( config->_file_name );
            if ( loader_setting == nullptr )
            {
                static std::string error = __FORMAT__( "config=[{}] can't find load setting", config->_file_name );
                throw std::runtime_error( error );
            }

            for ( auto& config_data : loader_setting->_config_data )
            {
                auto file_path = __FORMAT__( "{}{}.xml", config_data._path, config_data._name );

                // 加载配置
                LoadConfigFile( config, file_path, config_data._clear_type );

                // 设置版本号
                auto version_setting = _kf_version_config->FindSetting( config_data._name );
                if ( version_setting != nullptr )
                {
                    config->AddFileVersion( config_data._name, version_setting->_version );
                }
            }

            // 配置加载完成
            config->LoadComplete();
        }

        // 所有配置加载完成
        for ( auto& iter : _config_list )
        {
            auto config = iter.second;
            config->LoadAllComplete();
        }
    }

    const std::string& KFLoaderModule::CheckConfigFileNeedReload( KFConfig* config, const KFConfigData* config_data, StringSet& reload_list )
    {
        if ( !config_data->_can_reload )
        {
            return _invalid_string;
        }

        auto version_setting = _kf_version_config->FindSetting( config_data->_name );
        if ( version_setting == nullptr )
        {
            return _invalid_string;
        }

        // 如果父文件重新加载过了
        if ( !config_data->_parent_name.empty() &&
                reload_list.find( config_data->_parent_name ) != reload_list.end()  )
        {
            return version_setting->_version;
        }

        auto old_version = config->GetFileVersion( config_data->_name );
        if ( old_version != version_setting->_version )
        {
            return version_setting->_version;
        }

        return _invalid_string;
    }


    void KFLoaderModule::ReloadConfig()
    {
        __LOG_INFO__( "reload config start" );

        // 重新加载配置列表
        LoadConfigListAndVersion();

        auto load_all_ok = false;
        for ( auto& iter : _config_list )
        {
            auto config = iter.second;
            config->_load_ok = false;

            auto loader_setting = _kf_loader_config->FindSetting( config->_file_name );
            if ( loader_setting == nullptr )
            {
                continue;
            }

            // 加载的文件列表
            StringSet reload_list;
            for ( auto& config_data : loader_setting->_config_data )
            {
                auto new_version = CheckConfigFileNeedReload( config, &config_data, reload_list );
                if ( !new_version.empty() )
                {
                    continue;
                }

                // 加载配置
                auto file_path = __FORMAT__( "{}{}.xml", config_data._path, config_data._name );
                LoadConfigFile( config, file_path, config_data._clear_type );

                load_all_ok = true;
                config->AddFileVersion( config_data._name, new_version );
            }

            // 加载完成
            if ( config->_load_ok )
            {
                config->LoadComplete();
            }
        }

        if ( load_all_ok )
        {
            for ( auto& iter : _config_list )
            {
                auto config = iter.second;
                config->LoadAllComplete();
            }

            _kf_plugin_manage->AfterLoad();
        }

        __LOG_INFO__( "reload config ok" );
    }

    void KFLoaderModule::LoadConfigFile( KFConfig* config, const std::string& file_path, uint32 clear_type )
    {
        try
        {
            auto final_file_path = KFUtility::FormatConfigFile( file_path, KFGlobal::Instance()->_channel, KFGlobal::Instance()->_service );
            auto ok = config->LoadConfig( final_file_path, clear_type );
            if ( ok )
            {
                config->_load_ok = true;
                __LOG_INFO__( "load [{}] ok", final_file_path );
            }
            else
            {
                __LOG_ERROR__( "load [{}] failed", final_file_path );
            }
        }
        catch ( ... )
        {
            static std::string error = __FORMAT__( "load [{}] error", file_path );
            throw std::runtime_error( error );
        }
    }
}