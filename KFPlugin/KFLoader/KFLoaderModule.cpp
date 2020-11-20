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

    void KFLoaderModule::AddConfig( const std::string& name, KFConfig* kfconfig )
    {
        _config_list[ name ] = kfconfig;
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
            auto configfilepath = _config_path + "frameconfig.xml";
            LoadConfigFile( _kf_loader_config, configfilepath, KFLoaderEnum::ClearTypeAll );
        }
        {
            auto configfilepath = _config_path + "config.xml";
            LoadConfigFile( _kf_loader_config, configfilepath, KFLoaderEnum::ClearTypeNone );
        }

        {
            auto configfilepath = _config_path + "version.xml";
            LoadConfigFile( _kf_version_config, configfilepath, KFLoaderEnum::ClearTypeAll );
        }
    }

    void KFLoaderModule::LoadConfig()
    {
        // 读取配置文件列表
        LoadConfigListAndVersion();

        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            if ( kfconfig->_file_name.empty() )
            {
                continue;
            }

            auto kfsetting = _kf_loader_config->FindSetting( kfconfig->_file_name );
            if ( kfsetting == nullptr )
            {
                static std::string error = __FORMAT__( "config=[{}] can't find load setting", kfconfig->_file_name );
                throw std::runtime_error( error );
            }

            for ( auto& configdata : kfsetting->_config_data )
            {
                auto filepath = __FORMAT__( "{}{}.xml", configdata._path, configdata._name );

                // 加载配置
                LoadConfigFile( kfconfig, filepath, configdata._clear_type );

                // 设置版本号
                auto kfversionsetting = _kf_version_config->FindSetting( configdata._name );
                if ( kfversionsetting != nullptr )
                {
                    kfconfig->AddFileVersion( configdata._name, kfversionsetting->_version );
                }
            }

            // 配置加载完成
            kfconfig->LoadComplete();
        }

        // 所有配置加载完成
        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            kfconfig->LoadAllComplete();
        }
    }

    const std::string& KFLoaderModule::CheckConfigFileNeedReload( KFConfig* config, const KFConfigData* configdata, StringSet& reloadlist )
    {
        if ( !configdata->_can_reload )
        {
            return _invalid_string;
        }

        auto kfversionsetting = _kf_version_config->FindSetting( configdata->_name );
        if ( kfversionsetting == nullptr )
        {
            return _invalid_string;
        }

        // 如果父文件重新加载过了
        if ( !configdata->_parent_name.empty() &&
                reloadlist.find( configdata->_parent_name ) != reloadlist.end()  )
        {
            return kfversionsetting->_version;
        }

        auto oldversion = config->GetFileVersion( configdata->_name );
        if ( oldversion != kfversionsetting->_version )
        {
            return kfversionsetting->_version;
        }

        return _invalid_string;
    }


    void KFLoaderModule::ReloadConfig()
    {
        __LOG_INFO__( "reload config start" );

        // 重新加载配置列表
        LoadConfigListAndVersion();

        auto loadallok = false;
        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            kfconfig->_load_ok = false;

            auto kfconfigsetting = _kf_loader_config->FindSetting( kfconfig->_file_name );
            if ( kfconfigsetting == nullptr )
            {
                continue;
            }

            // 加载的文件列表
            StringSet reloadlist;
            for ( auto& configdata : kfconfigsetting->_config_data )
            {
                auto newversion = CheckConfigFileNeedReload( kfconfig, &configdata, reloadlist );
                if ( !newversion.empty() )
                {
                    continue;
                }

                // 加载配置
                auto filepath = __FORMAT__( "{}{}.xml", configdata._path, configdata._name );
                LoadConfigFile( kfconfig, filepath, configdata._clear_type );

                loadallok = true;
                kfconfig->AddFileVersion( configdata._name, newversion );
            }

            // 加载完成
            if ( kfconfig->_load_ok )
            {
                kfconfig->LoadComplete();
            }
        }

        if ( loadallok )
        {
            for ( auto& iter : _config_list )
            {
                auto kfconfig = iter.second;
                kfconfig->LoadAllComplete();
            }

            _kf_plugin_manage->AfterLoad();
        }

        __LOG_INFO__( "reload config ok" );
    }

    void KFLoaderModule::LoadConfigFile( KFConfig* config, const std::string& filepath, uint32 cleartype )
    {
        try
        {
            auto ok = config->LoadConfig( filepath, cleartype );
            if ( ok )
            {
                config->_load_ok = true;
                __LOG_INFO__( "load [{}] ok", filepath );
            }
            else
            {
                __LOG_ERROR__( "load [{}] failed", filepath );
            }
        }
        catch ( ... )
        {
            static std::string error = __FORMAT__( "load [{}] error", filepath );
            throw std::runtime_error( error );
        }
    }
}