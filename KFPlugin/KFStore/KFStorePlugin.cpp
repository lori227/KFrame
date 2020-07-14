#include "KFStorePlugin.hpp"
#include "KFStoreModule.hpp"
#include "KFGoodsModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFStorePlugin::Install()
    {
        __REGISTER_MODULE__( KFStore );
        __REGISTER_MODULE__( KFGoods );
    }

    void KFStorePlugin::UnInstall()
    {
        __UN_MODULE__( KFStore );
        __UN_MODULE__( KFGoods );
    }

    void KFStorePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_reset, KFResetInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_player, KFPlayerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_store, KFStoreInterface );
        __FIND_MODULE__( _kf_goods, KFGoodsInterface );
    }

    void KFStorePlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFStoreConfig );
        __KF_ADD_CONFIG__( KFGoodsConfig );
        __KF_ADD_CONFIG__( KFCountCostConfig );
        __KF_ADD_CONFIG__( KFElementConfig );
    }
}