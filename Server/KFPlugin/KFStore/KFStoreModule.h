#ifndef __KF_STORE_MODULE_H__
#define __KF_STORE__MODULE_H__

#include "KFrame.h"
#include "KFStoreInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFMailClient/KFMailClientInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFRelationClient/KFRelationClientInterface.h"

namespace KFrame
{
    class KFStoreModule : public KFStoreInterface
    {
    public:
        KFStoreModule() = default;
        ~KFStoreModule() = default;

        //初始化
        virtual void InitModule();

        //初始化
        virtual void BeforeRun();

        //关闭
        virtual void BeforeShut();


    protected:
        // 更新折扣
        __KF_UPDATE_DATA_FUNCTION__( OnDiscountUpdateCallBack );

        //购买商品
        __KF_MESSAGE_FUNCTION__( HandleBuyStoreReq );

        //赠送商品
        __KF_MESSAGE_FUNCTION__( HandleGiveStoreReq );

        // 请求商城信息
        __KF_MESSAGE_FUNCTION__( HandleQueryStoreInfoReq );

        // 操作心愿单请求
        __KF_MESSAGE_FUNCTION__( HandleSetWishOrderReq );

    private:
        // 购买商品
        uint32 BuyStoreResult( KFEntity* player, const std::string& buytype, uint32 shopid, uint32 num );
        bool CheckLimitItem( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime, uint32 buylimits );
        bool CheckOwnLimit( KFEntity* player, uint32 maxowns, uint32 num, const KFAgents* kfagents );
        void SetLimitInfo( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime );

        // 心愿单
        uint32 AddWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status );
        uint32 RemoveWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid );
        uint32 ModifyWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status );

    private:
        KFComponent* _kf_component{ nullptr };
    };

}



#endif//__KF_STORE_MODULE_H__