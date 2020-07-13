#include "KFConfigModule.hpp"

namespace KFrame
{
    static std::string _config_path = "./config/";
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    KFConfigModule::KFConfigModule()
    {
        _kf_config_config = __NEW_OBJECT__( KFConfigConfig );
        _kf_version_config = __NEW_OBJECT__( KFVersionConfig );
    }

    KFConfigModule::~KFConfigModule()
    {
        __DELETE_OBJECT__( _kf_config_config );
        __DELETE_OBJECT__( _kf_version_config );
    }

    void KFConfigModule::BeforeRun()
    {
        _kf_plugin_manage->RegisterCommandFunction( __STRING__( reloadconfig ), this, &KFConfigModule::ProcessReloadCommand );
    }

    void KFConfigModule::ShutDown()
    {
        _kf_plugin_manage->UnRegisterCommandFunction( __STRING__( reloadconfig ) );

        for ( auto& iter : _config_list )
        {
            delete iter.second;
        }
        _config_list.clear();
    }

    void KFConfigModule::ProcessReloadCommand( const StringVector& params )
    {
        ReloadConfig( _globbing_string );
    }

    void KFConfigModule::AddConfig( const std::string& name, KFConfig* kfconfig )
    {
        _config_list[ name ] = kfconfig;
    }

    KFConfig* KFConfigModule::FindConfig( const std::string& name )
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
    void KFConfigModule::LoadConfigListAndVersion()
    {
        {
            auto configfile = _config_path + "frameconfig.xml";
            LoadConfigFile( _kf_config_config, "config", configfile, KFConfigEnum::CanReload | KFConfigEnum::NeedClearData );
        }
        {
            auto configfile = _config_path + "config.xml";
            LoadConfigFile( _kf_config_config, "config", configfile, KFConfigEnum::CanReload );
        }

        {
            auto configfile = _config_path + "version.xml";
            LoadConfigFile( _kf_version_config, "version", configfile, KFConfigEnum::CanReload );
        }
    }

    void KFConfigModule::LoadConfig()
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

            auto kfsetting = _kf_config_config->FindSetting( kfconfig->_file_name );
            if ( kfsetting == nullptr )
            {
                __LOG_ERROR__( "config=[{}:{}] can't find setting", iter.first, kfconfig->_file_name );
                continue;
            }

            for ( auto& configdata : kfsetting->_config_data_list )
            {
                // 加载配置
                LoadConfigFile( kfconfig, configdata._file_name, configdata._file_path, configdata._load_mask );

                // 设置版本号
                auto kfversionsetting = _kf_version_config->FindSetting( configdata._file_name );
                if ( kfversionsetting != nullptr )
                {
                    kfconfig->SetVersion( kfversionsetting->_new_version );
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


    void KFConfigModule::ReloadConfig( const std::string& file )
    {
        __LOG_INFO__( "reload [{}] start", file );

        // 重新加载配置列表
        LoadConfigListAndVersion();

        auto loadallok = false;
        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            auto kfconfigsetting = _kf_config_config->FindSetting( kfconfig->_file_name );
            if ( kfconfigsetting == nullptr )
            {
                continue;
            }

            // 判断版本号是否相同
            auto loadok = false;
            auto childfilereload = false;
            for ( auto& configdata : kfconfigsetting->_config_data_list )
            {
                // 检查是否需要更新
                auto& newversion = _kf_version_config->CheckNeedReload( configdata._file_name );
                if ( !childfilereload )
                {
                    // 版本相同
                    if ( newversion.empty() )
                    {
                        continue;
                    }

                    // 不能重新加载
                    if ( !KFUtility::HaveBitMask<uint32>( configdata._load_mask, KFConfigEnum::CanReload ) )
                    {
                        continue;
                    }

                    if ( KFUtility::HaveBitMask<uint32>( configdata._load_mask, KFConfigEnum::ClearChildFile ) )
                    {
                        childfilereload = true;
                    }
                }

                // 加载配置
                LoadConfigFile( kfconfig, configdata._file_name, configdata._file_path, configdata._load_mask );

                loadok = true;
                loadallok = true;
                kfconfig->SetVersion( newversion );
            }

            // 加载完成
            if ( loadok )
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

        __LOG_INFO__( "reload [{}] ok", file );
    }

    void KFConfigModule::LoadConfigFile( KFConfig* config, const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        try
        {
            auto ok = config->LoadConfig( filename, filepath, loadmask );
            if ( ok )
            {
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