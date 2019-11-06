#include "KFConfigModule.hpp"

namespace KFrame
{
    static std::string _config_path = "./config/";
    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    KFConfigModule::KFConfigModule()
    {
        _kf_config_config = new KFConfigConfig();
        _kf_version_config = new KFVersionConfig();
    }

    KFConfigModule::~KFConfigModule()
    {
        __DELETE_OBJECT__( _kf_config_config );
        __DELETE_OBJECT__( _kf_version_config );
    }

    void KFConfigModule::BeforeRun()
    {
        _kf_plugin_manage->RegisterCommandFunction( __KF_STRING__( reloadconfig ), this, &KFConfigModule::ProcessReloadCommand );
    }

    void KFConfigModule::ShutDown()
    {
        _kf_plugin_manage->UnRegisterCommandFunction( __KF_STRING__( reloadconfig ) );

        for ( auto& iter : _config_list )
        {
            delete iter.second;
        }
        _config_list.clear();
    }

    void KFConfigModule::LoadConfigList()
    {
        auto configfile = _config_path + "config.xml";
        LoadConfigFile( _kf_config_config, "config", configfile, KFConfigEnum::CanReload | KFConfigEnum::NeedClearData );
    }

    void KFConfigModule::LoadConfig()
    {
        // 读取配置文件列表
        LoadConfigList();

        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            auto kfsetting = _kf_config_config->FindSetting( kfconfig->_file_name );
            if ( kfsetting == nullptr )
            {
                __LOG_ERROR__( "config=[{}:{}] can't find setting!", iter.first, kfconfig->_file_name );
                continue;
            }

            for ( auto& kfdata : kfsetting->_config_data_list )
            {
                LoadConfigFile( kfconfig, kfdata._file_name, kfdata._file_path, kfdata._load_mask );
            }
        }

        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            kfconfig->LoadAllComplete();
        }
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

    void KFConfigModule::ProcessReloadCommand( const VectorString& params )
    {
        ReloadConfig( _globbing_str );
    }

    void KFConfigModule::ReloadConfig( const std::string& file )
    {
        __LOG_INFO__( "reload [{}] start!", file );

        // 重新加载配置列表
        LoadConfigList();

        auto configfile = _config_path + "_xmlversion.xml";
        LoadConfigFile( _kf_version_config, "version", configfile, KFConfigEnum::NeedClearData | KFConfigEnum::CanReload );

        bool loadok = false;
        for ( auto& iter : _config_list )
        {
            auto kfconfig = iter.second;
            auto kfconfigsetting = _kf_config_config->FindSetting( kfconfig->_file_name );
            if ( kfconfigsetting == nullptr )
            {
                continue;
            }

            // 判断版本号是否相同
            for ( auto& kfdata : kfconfigsetting->_config_data_list )
            {
                if ( KFUtility::HaveBitMask<uint32>( kfdata._load_mask, KFConfigEnum::CanReload ) )
                {
                    auto kfversionsetting = _kf_version_config->FindSetting( kfdata._file_name );
                    if ( kfversionsetting == nullptr || kfconfig->CheckVersion( kfdata._file_name, kfversionsetting->_version ) )
                    {
                        continue;
                    }

                    loadok = true;
                    LoadConfigFile( kfconfig, kfdata._file_name, kfdata._file_path, kfdata._load_mask );
                }
            }
        }

        if ( loadok )
        {
            for ( auto& iter : _config_list )
            {
                auto kfconfig = iter.second;
                kfconfig->LoadAllComplete();
            }

            _kf_plugin_manage->AfterLoad();
        }

        __LOG_INFO__( "reload [{}] ok!", file );
    }

    void KFConfigModule::LoadConfigFile( KFConfig* config, const std::string& filename, const std::string& filepath, uint32 loadmask )
    {
        try
        {
            auto ok = config->LoadConfig( filename, filepath, loadmask );
            if ( ok )
            {
                config->LoadComplete();
                __LOG_INFO__( "load [{}] ok!", filepath );
            }
        }
        catch ( ... )
        {
            static std::string error = __FORMAT__( "load [{}] failed!", filepath );
            throw std::runtime_error( error );
        }
    }
}