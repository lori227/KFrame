#include "KFStoreModule.hpp"

namespace KFrame
{
    void KFStoreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        //////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFStoreModule, KFMsg::MSG_STORE_BUY_GOODS_REQ, KFMsg::MsgStoreBuyGoodsReq, HandleStoreBuyGoodsReq );
    }

    void KFStoreModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_STORE_BUY_GOODS_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleStoreBuyGoodsReq, KFMsg::MsgStoreBuyGoodsReq )
    {
        __ROUTE_FIND_PLAYER__;

        auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfmsg->storeid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreSettingError, kfmsg->storeid() );
        }

        auto result = _kf_goods->BuyGoods( player, kfmsg->storeid(), kfmsg->goodsid(), kfmsg->buycount() );
        return _kf_display->DelayToClient( player, result );
    }
}