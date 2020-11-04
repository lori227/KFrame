#include "KFGoodsModule.hpp"
#include "KFProtocol/KFProtocol.h"

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
            if ( kfsetting == nullptr || kfsetting->_limit_buy_time_id != timeid || !kfsetting->IsLimitBuy() )
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

    uint32 KFGoodsModule::BuyGoods( KFEntity* player, uint32 goodsid, uint32 buycount )
    {
        auto kfsetting = KFGoodsConfig::Instance()->FindSetting( goodsid );
        if ( kfsetting == nullptr )
        {
            return KFMsg::StoreGoodsSettingError;
        }

        // 判断购买数量
        if ( buycount == _invalid_int )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 不在购买时间内
        if ( !kfsetting->CheckInBuyTime( KFGlobal::Instance()->_real_time ) )
        {
            return KFMsg::StoreGoodsBuyTimeError;
        }

        // 判断包裹是否满了
        auto& checkname = player->CheckAddElement( &kfsetting->_goods_data, buycount, __FUNC_LINE__ );
        if ( !checkname.empty() )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 判断价钱
        auto costprice = kfsetting->CalcBuyPrice( KFGlobal::Instance()->_real_time );
        auto& dataname = player->RemoveElement( costprice, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        if ( !dataname.empty() )
        {
            return KFMsg::StoreLackCost;
        }

        // 判断如果是限购商品
        if ( kfsetting->IsLimitBuy() )
        {
            auto hasbuycount = player->Get<uint32>( __STRING__( goods ), goodsid, __STRING__( count ) );
            if ( hasbuycount + buycount > kfsetting->_limit_buy_count )
            {
                return KFMsg::StoreOutOfLimits;
            }

            player->UpdateRecord( __STRING__( goods ), goodsid, __STRING__( count ), KFEnum::Add, buycount );
        }

        // 添加商品
        player->AddElement( &kfsetting->_goods_data, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    std::tuple<uint32, uint32> KFGoodsModule::RandGoods( KFEntity* player, uint32 groupid, UInt32Set& excludelist )
    {
        auto kfgoodsweightdata = KFGoodsConfig::Instance()->RandGroupGoods( groupid, excludelist );
        if ( kfgoodsweightdata == nullptr )
        {
            return std::make_tuple( 0u, 0u );
        }

        return std::make_tuple( kfgoodsweightdata->_id, kfgoodsweightdata->_refresh_count );
    }
}
