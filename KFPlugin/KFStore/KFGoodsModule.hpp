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
#include "KFReset/KFResetInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFConfig/KFGoodsConfig.hpp"

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
        virtual uint32 BuyGoods( EntityPtr player, uint32 storeid, uint32 goodsid, uint32 buycount );

    protected:
        // 刷新限购商品
        __KF_RESET_FUNCTION__( OnResetRefreshGoods );

    protected:
        // 获得购买价格
        const KFElements* CalcBuyPrice( const KFGoodsSetting* kfsetting, uint64 now_time );

    };

}

#endif