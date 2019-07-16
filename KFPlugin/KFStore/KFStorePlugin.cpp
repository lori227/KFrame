#include "KFStorePlugin.hpp"
#include "KFStoreModule.hpp"

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
}