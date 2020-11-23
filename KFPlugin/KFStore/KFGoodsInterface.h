#ifndef __KF_GOODS_INTERFACE_H__
#define __KF_GOODS_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFGoodsInterface : public KFModule
    {
    public:
        // 购买商品
        virtual uint32 BuyGoods( KFEntity* player, uint32 storeid, uint32 goodsid, uint32 buycount ) = 0;
    };
    ///////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_goods, KFGoodsInterface );
    ///////////////////////////////////////////////////////////////////////////////////////
}

#endif