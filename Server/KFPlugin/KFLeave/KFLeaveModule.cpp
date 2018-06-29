#include "KFEnterModule.h"
#include "KFEnterConfig.h"
#include "KFKernel/KFKernelInterface.h"

namespace KFrame
{
    KFEnterModule::KFEnterModule()
    {

    }

    KFEnterModule::~KFEnterModule()
    {
    }

    void KFEnterModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////
        _kf_config->AddConfig( _kf_enter_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
    }

    void KFEnterModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( typeid( KFEnterModule ).name(), this, &KFEnterModule::EnterGameWorld );
    }


    void KFEnterModule::BeforeShut()
    {
        _kf_config->RemoveConfig( _kf_plugin->_plugin_name );

        _kf_player->UnRegisterEnterFunction( typeid( KFEnterModule ).name() );
    }

    void KFEnterModule::EnterGameWorld( KFEntity* player )
    {


    }
}