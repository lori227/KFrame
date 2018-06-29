#include "KFTitleModule.h"
#include "KFTitleConfig.h"
#include "KFTitleField.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFComponent/KFComponentInterface.h"
#include "KFNetFunction/KFNetFunctionInterface.h"
namespace KFrame
{
    KFTitleModule::KFTitleModule()
    {
        _kf_player_logic = nullptr;
    }

    KFTitleModule::~KFTitleModule()
    {
    }

    void KFTitleModule::InitMoudle()
    {
        ///////////////////////////////////////////////////////////////////////////////
        _kf_config->AddConfig( _kf_title_config, _kf_plugin->_config, true );
    }

    void KFTitleModule::BeforeRun()
    {
        _kf_player_logic = _kf_component->FindComponent( KFField::_player );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
}