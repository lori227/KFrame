#include "KFStorePlugin.hpp"
#include "KFStoreModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFStorePlugin::Install()
    {
        __REGISTER_MODULE__( KFStore );
    }

    void KFStorePlugin::UnInstall()
    {
        __UN_MODULE__( KFStore );
    }

    void KFStorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
    }

    void KFStorePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFStoreConfig );
    }
}