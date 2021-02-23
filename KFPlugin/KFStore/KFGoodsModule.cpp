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
            auto setting = KFGoodsConfig::Instance()->FindSetting( kfgoods->GetKeyID() );
            if ( setting == nullptr || setting->_limit_time_id != timeid )
            {
                continue;
            }

            removelist.insert( setting->_id );
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
        auto setting = KFGoodsConfig::Instance()->FindSetting( goodsid );
        if ( setting == nullptr )
        {
            return KFMsg::StoreGoodsSettingError;
        }

        if ( setting->_store_id != storeid )
        {
            return KFMsg::StoreNotHaveGoods;
        }

        // 判断购买数量
        if ( buycount == _invalid_int )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 不在购买时间内
        if ( !KFDate::CheckInTime( setting->_buy_start_time, setting->_buy_finish_time, KFGlobal::Instance()->_real_time ) )
        {
            return KFMsg::StoreGoodsBuyTimeError;
        }

        // 判断包裹是否满了
        auto& checkname = player->CheckAddElement( &setting->_buy_data, buycount, __FUNC_LINE__ );
        if ( !checkname.empty() )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 判断价钱
        auto costprice = CalcBuyPrice( setting, KFGlobal::Instance()->_real_time );
        auto& data_name = player->RemoveElement( costprice, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        if ( !data_name.empty() )
        {
            return KFMsg::StoreLackCost;
        }

        // 判断如果是限购商品
        if ( setting->_limit_time_id != 0u )
        {
            auto hasbuycount = player->Get<uint32>( __STRING__( goods ), goodsid, __STRING__( count ) );
            if ( hasbuycount + buycount > setting->_limit_count )
            {
                return KFMsg::StoreOutOfLimits;
            }

            player->UpdateRecord( __STRING__( goods ), goodsid, __STRING__( count ), KFEnum::Add, buycount );
        }

        // 添加商品
        player->AddElement( &setting->_buy_data, buycount, __STRING__( goods ), goodsid, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    const KFElements* KFGoodsModule::CalcBuyPrice( const KFGoodsSetting* setting, uint64 now_time )
    {
        if ( KFDate::CheckInTime( setting->_discount_start_time, setting->_discount_finish_time, now_time ) )
        {
            return &setting->_discount;
        }

        return &setting->_price;
    }
}
