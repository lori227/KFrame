#ifndef __KF_STORE_MODULE_H__
#define __KF_STORE_MODULE_H__

/************************************************************************
//    @Module			:    商城系统
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2019-7-27
************************************************************************/

#include "KFStoreInterface.h"
#include "KFGoodsInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFReset/KFResetInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFConfig/KFStoreConfig.hpp"
#include "KFConfig/KFGoodsConfig.hpp"

namespace KFrame
{
    class KFStoreModule : public KFStoreInterface
    {
    public:
        KFStoreModule() = default;
        ~KFStoreModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 请求购买商城道具
        __KF_MESSAGE_FUNCTION__( HandleStoreBuyGoodsReq, KFMsg::MsgStoreBuyGoodsReq );

    protected:
        // 玩家组件上下文
        std::shared_ptr<KFComponent> _component = nullptr;
    };

}

#endif