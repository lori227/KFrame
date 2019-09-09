#include "KFPlugin.h"

namespace KFrame
{
    KFPlugin::KFPlugin()
    {
        _sort = 0;
        _modules.clear();
    }

    KFPlugin::~KFPlugin()
    {
        for ( auto& iter : _modules )
        {
            __DELETE_OBJECT__( iter.second );
        }
        _modules.clear();
    }

    void KFPlugin::InitModule()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->InitModule();
        }
    }

    void KFPlugin::LoadConfig()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            iter.second->LoadConfig();
        }
    }

    void KFPlugin::BeforeRun()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->BeforeRun();
        }
    }

    void KFPlugin::BeforeShut()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->BeforeShut();
        }
    }

    void KFPlugin::ShutDown()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->ShutDown();
        }
    }

    void KFPlugin::AfterShut()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->AfterShut();
        }
    }

    void KFPlugin::PrepareRun()
    {
        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->PrepareRun();
        }
    }

    void KFPlugin::Reload()
    {
        InitModule();
        BeforeRun();
        PrepareRun();

        for ( auto& iter : _modules )
        {
            _init_module = iter.second;
            _init_module->LoadData();
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    KFModule* KFPlugin::FindModule( const std::string& name )
    {
        auto iter = _modules.find( name );
        if ( iter == _modules.end() )
        {
            return nullptr;
        }

        return iter->second;
    }

    void KFPlugin::BindModule( const std::string& name, KFModule* module )
    {
        module->_kf_plugin = this;
        module->_class_name = name;
        module->_plugin_name = _plugin_name;
        module->_kf_plugin_manage = _kf_plugin_manage;
        _modules[ name ] = module;
    }

    void KFPlugin::UnBindModule( const std::string& name, bool savedata )
    {
        auto iter = _modules.find( name );
        if ( iter == _modules.end() )
        {
            return ;
        }

        auto kfmodule = iter->second;
        if ( savedata )
        {
            kfmodule->SaveData();

            kfmodule->BeforeShut();
            kfmodule->ShutDown();
            kfmodule->AfterShut();
        }

        __DELETE_OBJECT__( kfmodule );
        _modules.erase( iter );
    }
}
