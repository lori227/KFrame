#include "KFPlugin.h"

namespace KFrame
{
    KFPlugin::KFPlugin()
    {
        _sort = 0;
        _kf_module = nullptr;
    }

    KFPlugin::~KFPlugin()
    {
        __DELETE_OBJECT__( _kf_module );
    }

    void KFPlugin::InitModule()
    {
        _kf_module->InitModule();
    }

    void KFPlugin::LoadConfig()
    {
        _kf_module->LoadConfig();
    }

    void KFPlugin::BeforeRun()
    {
        _kf_module->BeforeRun();
    }

    void KFPlugin::BeforeShut()
    {
        _kf_module->BeforeShut();
    }

    void KFPlugin::ShutDown()
    {
        _kf_module->ShutDown();
    }

    void KFPlugin::AfterShut()
    {
        _kf_module->AfterShut();
    }

    void KFPlugin::PrepareRun()
    {
        _kf_module->PrepareRun();
    }

    void KFPlugin::Reload()
    {
        _kf_module->InitModule();
        _kf_module->BeforeRun();
        _kf_module->PrepareRun();
        _kf_module->LoadData();
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    KFModule* KFPlugin::FindModule( const std::string& name )
    {
        if ( name == _kf_module->_class_name )
        {
            return _kf_module;
        }

        return nullptr;
    }

    void KFPlugin::BindModule( const std::string& name, KFModule* module )
    {
        _kf_module = module;
        _kf_module->_kf_plugin = this;
        _kf_module->_class_name = name;
        _kf_module->_plugin_name = _plugin_name;
        _kf_module->_kf_plugin_manage = _kf_plugin_manage;
    }

    void KFPlugin::UnBindModule( bool savedata )
    {
        if ( savedata )
        {
            _kf_module->SaveData();

            _kf_module->BeforeShut();
            _kf_module->ShutDown();
            _kf_module->AfterShut();
        }

        __DELETE_OBJECT__( _kf_module );
    }
}
