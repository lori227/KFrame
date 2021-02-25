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
        auto goods_record = player->Find( __STRING__( goods ) );
        if ( goods_record == nullptr )
        {
            return;
        }

        UInt64Set remove_list;
        for ( auto goods_data = goods_record->First(); goods_data != nullptr; goods_data = goods_record->Next() )
        {
            // 永久限购
            auto setting = KFGoodsConfig::Instance()->FindSetting( goods_data->GetKeyID() );
            if ( setting == nullptr || setting->_limit_time_id != timeid )
            {
                continue;
            }

            remove_list.insert( setting->_id );
        }

        if ( remove_list.empty() )
        {
            return;
        }

        for ( auto goods_id : remove_list )
        {
            player->RemoveRecord( goods_record, goods_id, false );
        }
        _kf_display->DelayToClient( player, KFMsg::StoreGoodsRefreshOk );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint32 KFGoodsModule::BuyGoods( EntityPtr player, uint32 store_id, uint32 goods_id, uint32 buy_count )
    {
        auto setting = KFGoodsConfig::Instance()->FindSetting( goods_id );
        if ( setting == nullptr )
        {
            return KFMsg::StoreGoodsSettingError;
        }

        if ( setting->_store_id != store_id )
        {
            return KFMsg::StoreNotHaveGoods;
        }

        // 判断购买数量
        if ( buy_count == _invalid_int )
        {
            return KFMsg::StoreBuyCountError;
        }

        // 不在购买时间内
        if ( !KFDate::CheckInTime( setting->_buy_start_time, setting->_buy_finish_time, KFGlobal::Instance()->_real_time ) )
        {
            return KFMsg::StoreGoodsBuyTimeError;
        }

        // 判断包裹是否满了
        auto& check_name = player->CheckAddElement( &setting->_buy_data, buy_count, __FUNC_LINE__ );
        if ( !check_name.empty() )
        {
            return KFMsg::ItemBagIsFull;
        }

        // 判断价钱
        auto cost_price = CalcBuyPrice( setting, KFGlobal::Instance()->_real_time );
        auto& data_name = player->RemoveElement( cost_price, buy_count, __STRING__( goods ), goods_id, __FUNC_LINE__ );
        if ( !data_name.empty() )
        {
            return KFMsg::StoreLackCost;
        }

        // 判断如果是限购商品
        if ( setting->_limit_time_id != 0u )
        {
            auto already_buy_count = player->Get<uint32>( __STRING__( goods ), goods_id, __STRING__( count ) );
            if ( already_buy_count + buy_count > setting->_limit_count )
            {
                return KFMsg::StoreOutOfLimits;
            }

            player->UpdateRecord( __STRING__( goods ), goods_id, __STRING__( count ), KFEnum::Add, buy_count );
        }

        // 添加商品
        player->AddElement( &setting->_buy_data, buy_count, __STRING__( goods ), goods_id, __FUNC_LINE__ );
        return KFMsg::StoreBuyOK;
    }

    const KFElements* KFGoodsModule::CalcBuyPrice( std::shared_ptr<const KFGoodsSetting> setting, uint64 now_time )
    {
        if ( KFDate::CheckInTime( setting->_discount_start_time, setting->_discount_finish_time, now_time ) )
        {
            return &setting->_discount;
        }

        return &setting->_price;
    }
}
