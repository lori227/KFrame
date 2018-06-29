#include "KFOptionModule.h"
#include "KFOptionConfig.h"

namespace KFrame
{
    KFOptionModule::KFOptionModule()
    {
    }

    KFOptionModule::~KFOptionModule()
    {
    }

    void KFOptionModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_option_config, true );
    }

    void KFOptionModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
    }

    ////////////////////////////////////////////////////////////////
    const std::string& KFOptionModule::GetString( const std::string& name, uint32 key /* = 0 */ )
    {
        return _kf_option_config->FindOption( name, key );
    }
}