#ifndef __KF_GOODS_MODULE_H__
#define __KF_GOODS_MODULE_H__

/************************************************************************
//    @Module			:    商品刷新系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-27
************************************************************************/

#include "KFGoodsInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFGoodsConfig.hpp"

namespace KFrame
{
    class KFGoodsModule : public KFGoodsInterface
    {
    public:
        KFGoodsModule() = default;
        ~KFGoodsModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        // 购买商品
        virtual uint32 BuyGoods( KFEntity* player, uint32 goodsid, uint32 buycount );

        // 随机一个商品
        virtual std::tuple<uint32, uint32> RandGoods( KFEntity* player, uint32 groupid, UInt32Set& excludelist );

    protected:
        // 刷新限购商品
        __KF_RESET_FUNCTION__( OnResetRefreshGoods );
    };

}

#endif