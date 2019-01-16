#ifndef __KF_STORE_MODULE_H__
#define __KF_STORE__MODULE_H__

#include "KFrame.h"
#include "KFStoreInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFStoreModule : public KFStoreInterface
    {
    public:
        KFStoreModule() = default;
        ~KFStoreModule() = default;

        // 初始化
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();


    protected:
        // 购买商品
        __KF_MESSAGE_FUNCTION__( HandleBuyStoreReq );

        // 请求商城信息
        __KF_MESSAGE_FUNCTION__( HandleQueryStoreInfoReq );

    private:
        // 购买商品
        uint32 BuyStoreResult( KFEntity* player, const std::string& buytype, uint32 shopid, uint32 num );
        bool CheckLimitItem( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime, uint32 buylimits );
        bool CheckOwnLimit( KFEntity* player, uint32 maxowns, uint32 num, const KFElements* kfagents );
        void SetLimitInfo( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime );

        // 心愿单
        uint32 AddWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status );
        uint32 RemoveWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid );
        uint32 ModifyWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status );
    };

}

#endif//__KF_STORE_MODULE_H__