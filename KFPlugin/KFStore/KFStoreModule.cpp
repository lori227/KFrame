#include "KFStoreModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFStoreModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_PLAYER_ENTER__( &KFStoreModule::OnEnterStoreModule );
        __REGISTER_PLAYER_LEAVE__( &KFStoreModule::OnLeaveStoreModule );

        __REGISTER_RESET__( __STRING__( store ), &KFStoreModule::OnResetRefreshStoreCount );
        //////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_STORE_BUY_GOODS_REQ, &KFStoreModule::HandleStoreBuyGoodsReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_STORE_REFRESH_REQ, &KFStoreModule::HandleStoreRefreshReq );
    }

    void KFStoreModule::BeforeShut()
    {
        __UN_RESET__( __STRING__( store ) );
        __UN_TIMER_0__();
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();
        //////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_STORE_BUY_GOODS_REQ );
        __UN_MESSAGE__( KFMsg::MSG_STORE_REFRESH_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleStoreBuyGoodsReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStoreBuyGoodsReq );

        auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfmsg.storeid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreSettingError, kfmsg.storeid() );
        }

        auto result = _kf_goods->BuyGoods( player, kfmsg.storeid(), kfmsg.goodsid(), kfmsg.buycount() );
        return _kf_display->DelayToClient( player, result );
    }
}