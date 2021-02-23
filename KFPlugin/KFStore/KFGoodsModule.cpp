#include "KFGoodsModule.hpp"

namespace KFrame
{
    void KFGoodsModule::BeforeRun()
    {
        __REGISTER_RESET__( __STRING__( goods ), &KFGoodsModule::OnResetRefreshGoods );
    }

    void KFGoodsModule::BeforeShut()
    {
        __UN_RESET__( __STRING__( goods ) );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_RESET_FUNCTION__( KFGoodsModule::OnResetRefreshGoods )
    {
        auto kfgoodsrecord = player->Find( __STRING__( goods ) );
        if ( kfgoodsrecord == nullptr )
        {
            return;
        }

        UInt64Set removelist;
        for ( auto kfgoods = kfgoodsrecord->First(); kfgoods != nullptr; kfgoods = kfgoodsrecord->Next() )
        {
            // 永久限购
            auto kfsetting = KFGoodsConfig::Instance()->FindSetting( kfgoods->GetKeyID() );
            if ( kfsetting == nullptr || kfsetting->_limit_time_id != timeid )
            {
                continue;
            }

            removelist.insert( kfsetting->_id );
        }

        if ( removelist.empty() )
        {
            return;
        }

        for ( auto goodsid : removelist )
        {
            player->RemoveRecord( kfgoodsrecord, goodsid, false );
        }
        _kf_display->DelayToClient( player, KFMsg::StoreGoodsRefreshOk );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFGoodsModule::BuyGoods( EntityPtr player, uint32 storeid, uint32 goodsid, uint32 buycount )
    {
        auto kfsetting = KFGoodsConfig::Instance()->FindSetting( goodsid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::StoreGoodsSettingError;
        }

        if ( kfsetting->_store_id != storeid )
        {
            return KFMsg::StoreNotHaveGoods;
        }

        // 判断购买数量
        if ( buycount == _invalid_int )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 不在购买时间内
        if ( !KFDate::CheckInTime( kfsetting->_buy_start_time, kfsetting->_buy_finish_time, KFGlobal::Instance()->_real_time ) )
        {
            return KFMsg::StoreGoodsBuyTimeError;
        }

        // 判断包裹是否满了
        auto& checkname = player->CheckAddElement( &kfsetting->_buy_data, buycount, __FUNC_LINE__ );
        if ( !checkname.empty() )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 判断价钱
        auto costprice = CalcBuyPrice( kfsetting, KFGlobal::Instance()->_real_time );
        auto& data_name = player->RemoveElement( costprice, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        if ( !data_name.empty() )
        {
            return KFMsg::StoreLackCost;
        }

        // 判断如果是限购商品
        if ( kfsetting->_limit_time_id != 0u )
        {
            auto hasbuycount = player->Get<uint32>( __STRING__( goods ), goodsid, __STRING__( count ) );
            if ( hasbuycount + buycount > kfsetting->_limit_count )
            {
                return KFMsg::StoreOutOfLimits;
            }

            player->UpdateRecord( __STRING__( goods ), goodsid, __STRING__( count ), KFEnum::Add, buycount );
        }

        // 添加商品
        player->AddElement( &kfsetting->_buy_data, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    const KFElements* KFGoodsModule::CalcBuyPrice( const KFGoodsSetting* kfsetting, uint64 now_time )
    {
        if ( KFDate::CheckInTime( kfsetting->_discount_start_time, kfsetting->_discount_finish_time, now_time ) )
        {
            return &kfsetting->_discount;
        }

        return &kfsetting->_price;
    }
}
