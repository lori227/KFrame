#ifndef __KF_STORE_INTERFACE_H__
#define __KF_STORE_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFStoreInterface : public KFModule
    {
    public:
        // 刷新商店
        virtual void RefreshGoods( KFEntity* player, uint32 storeid ) = 0;

        // 删除随机商店
        virtual void RemoveStore( KFEntity* player, uint32 storeid ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_store, KFStoreInterface );
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif