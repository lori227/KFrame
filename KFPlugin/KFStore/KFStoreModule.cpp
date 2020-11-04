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

        // 固定商店
        if ( kfsetting->_refresh_type == KFMsg::RefreshNever )
        {
            // 判断是否存在该商品
            if ( !kfsetting->IsHaveGoods( kfmsg.goodsid() ) )
            {
                return _kf_display->SendToClient( player, KFMsg::StoreNotHaveGoods, kfmsg.goodsid() );
            }

            auto result = _kf_goods->BuyGoods( player, kfmsg.goodsid(), kfmsg.buycount() );
            return _kf_display->DelayToClient( player, result );
        }

        // 随机刷新商店
        auto kfstore = player->Find( __STRING__( store ), kfmsg.storeid() );
        if ( kfstore == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotRefresh, kfmsg.storeid() );
        }

        auto kfgoods = kfstore->Find( __STRING__( goods ), kfmsg.index() );
        if ( kfgoods == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreBuyIndexError, kfmsg.index() );
        }

        auto goodsid = kfgoods->Get<uint32>( __STRING__( id ) );
        if ( goodsid != kfmsg.goodsid() )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotHaveGoods, kfmsg.goodsid() );
        }

        auto buycount = kfgoods->Get<uint32>( __STRING__( count ) );
        buycount = __MIN__( buycount, kfmsg.buycount() );

        auto result = _kf_goods->BuyGoods( player, kfmsg.goodsid(), kfmsg.buycount() );
        if ( result == KFMsg::Ok )
        {
            // 减少商品数量
            player->UpdateObject( kfgoods, __STRING__( count ), KFEnum::Dec, buycount );
        }

        _kf_display->DelayToClient( player, result );
    }

    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleStoreRefreshReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgStoreRefreshReq );
        auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfmsg.storeid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreSettingError, kfmsg.storeid() );
        }

        if ( !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshManual ) )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreNotRefreshManual );
        }

        auto kfstorerecord = player->Find( __STRING__( store ) );
        switch ( kfmsg.refreshtype() )
        {
        case KFMsg::RefreshByFree:
        {
            // 判断刷新时间
            auto refreshtime = kfstorerecord->Get( kfmsg.storeid(), __STRING__( time ) );
            if ( refreshtime > KFGlobal::Instance()->_real_time )
            {
                return _kf_display->SendToClient( player, KFMsg::StoreRefreshTimeError );
            }

            // 保存时间
            UpdateStoreRefreshTime( player, kfsetting, kfstorerecord );
        }
        break;
        case KFMsg::RefreshByCost:
        {
            auto refreshcount = kfstorerecord->Get( kfmsg.storeid(), __STRING__( refresh ) );
            auto tupldata = StoreRefreshCostElemnts( player, kfsetting, refreshcount + 1u );
            auto result = std::get<0>( tupldata );
            auto refreshtimeid = std::get<1>( tupldata );
            if ( result != KFMsg::Ok )
            {
                return _kf_display->SendToClient( player, result );
            }

            // 设置刷新数量
            if ( refreshtimeid != 0u )
            {
                player->UpdateRecord( kfstorerecord, kfsetting->_id, __STRING__( refresh ), KFEnum::Add, 1u );
            }

            // 是否要重置时间
            if ( kfsetting->_is_refresh_reset_time )
            {
                UpdateStoreRefreshTime( player, kfsetting, kfstorerecord );
            }
        }
        break;
        default:
            return _kf_display->SendToClient( player, KFMsg::StoreRefreshTypeError );
            break;
        }

        // 刷新商品
        StoreRefreshGoods( player, kfsetting, kfstorerecord );
    }

    std::tuple< uint32, uint32 > KFStoreModule::StoreRefreshCostElemnts( KFEntity* player, const KFStoreSetting* kfsetting, uint32 refreshcount )
    {
        if ( kfsetting->_refresh_cost_list.empty() )
        {
            return std::make_tuple( KFMsg::StoreRefreshCostEmpty, 0u );
        }

        // 刷新花费, 可多种消耗, 在前面的优先扣除
        for ( auto& costid : kfsetting->_refresh_cost_list )
        {
            auto tupledata = KFCountCostConfig::Instance()->GetCostByCount( costid, refreshcount );
            auto costelement = std::get<0>( tupledata );
            auto refreshtimeid = std::get<1>( tupledata );
            if ( costelement == nullptr )
            {
                continue;
            }

            auto& dataname = player->RemoveElement( costelement, 1u, __STRING__( store ), kfsetting->_id, __FUNC_LINE__ );
            if ( dataname.empty() )
            {
                return std::make_tuple( KFMsg::Ok, refreshtimeid );
            }
        }

        return std::make_tuple( KFMsg::DataNotEnough, 0u );
    }

    void KFStoreModule::UpdateStoreRefreshTime( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord )
    {
        auto nowtime = KFGlobal::Instance()->_real_time;
        auto nexttime = _kf_reset->CalcNextResetTime( player, kfsetting->_refresh_time_id );
        player->UpdateRecord( kfstorerecord, kfsetting->_id, __STRING__( time ), KFEnum::Set, nexttime );

        // 取消定时器
        __UN_TIMER_2__( player->GetKeyID(), kfsetting->_id );

        // 添加新的定时器
        StartRefreshStoreTimer( player, kfsetting->_id, nexttime );
    }

    void KFStoreModule::RefreshGoods( KFEntity* player, uint32 storeid )
    {
        auto kfsetting = KFStoreConfig::Instance()->FindSetting( storeid );
        if ( kfsetting == nullptr || !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshAuto ) )
        {
            return;
        }

        auto refreshtime = player->Get( __STRING__( store ), kfsetting->_id, __STRING__( time ) );
        StartRefreshStoreTimer( player, kfsetting->_id, refreshtime );
    }

    void KFStoreModule::StoreRefreshGoods( KFEntity* player, const KFStoreSetting* kfsetting, KFData* kfstorerecord )
    {
        if ( kfsetting->_refresh_group_count.empty() )
        {
            return;
        }

        // 清空商店的商品列表
        auto kfstoregoods = kfstorerecord->Find( kfsetting->_id, __STRING__( goods ) );
        player->ClearRecord( kfstoregoods, false );

        // 刷新商品( 每次商品组刷新随机数量个商品 )
        for ( auto& tupledata : kfsetting->_refresh_group_count )
        {
            auto groupid = std::get<0>( tupledata );
            auto mincount = std::get<1>( tupledata );
            auto maxcount = std::get<2>( tupledata );

            UInt32Set excludelist;
            auto count = KFGlobal::Instance()->RandRange( mincount, maxcount, 1u );
            for ( auto i = 0u; i < count; ++i )
            {
                auto tupledata = _kf_goods->RandGoods( player, groupid, excludelist );
                auto goodsid = std::get<0>( tupledata );
                auto goodscount = std::get<1>( tupledata );
                if ( goodsid == 0u || goodscount == 0 )
                {
                    continue;
                }

                auto kfgoods = player->CreateData( kfstoregoods );
                kfgoods->Set( __STRING__( id ), goodsid );
                kfgoods->Set( __STRING__( count ), goodscount );
                player->AddRecord( kfstoregoods, kfstoregoods->Size() + 1u, kfgoods );

                // 判断是否需要排除相同商品
                if ( kfsetting->_random_type == KFMsg::ExcludeRandom )
                {
                    excludelist.insert( goodsid );
                }
            }
        }

        _kf_display->DelayToClient( player, KFMsg::StoreRefreshOk );
    }

    void KFStoreModule::RemoveStore( KFEntity* player, uint32 storeid )
    {
        player->RemoveRecord( __STRING__( store ), storeid );
    }

    __KF_PLAYER_ENTER_FUNCTION__( KFStoreModule::OnEnterStoreModule )
    {
        auto kfstorerecord = player->Find( __STRING__( store ) );
        for ( auto& iter : KFStoreConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !KFUtility::HaveBitMask( kfsetting->_refresh_type, ( uint32 )KFMsg::RefreshAuto ) )
            {
                continue;
            }

            // 启动定时器
            auto refreshtime = kfstorerecord->Get( kfsetting->_id, __STRING__( time ) );
            StartRefreshStoreTimer( player, kfsetting->_id, refreshtime );
        }
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFStoreModule::OnLeaveStoreModule )
    {
        __UN_TIMER_1__( player->GetKeyID() );
    }

    void KFStoreModule::StartRefreshStoreTimer( KFEntity* player, uint32 storeid, uint64 refreshtime )
    {
        auto intervaltime = ( refreshtime - __MIN__( refreshtime, KFGlobal::Instance()->_real_time ) ) * KFTimeEnum::OneSecondMicSecond;
        intervaltime = __MAX__( 1, intervaltime );
        __LIMIT_TIMER_2__( player->GetKeyID(), storeid, intervaltime, 1u, &KFStoreModule::OnTimerRefreshStore );
    }

    __KF_TIMER_FUNCTION__( KFStoreModule::OnTimerRefreshStore )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return;
        }

        auto kfsetting = KFStoreConfig::Instance()->FindSetting( subid );
        if ( kfsetting == nullptr || kfsetting->_refresh_time_id == 0u )
        {
            return;
        }

        // 保存下次时间
        auto kfstorerecord = player->Find( __STRING__( store ) );
        auto nexttime = _kf_reset->CalcNextResetTime( player, kfsetting->_refresh_time_id );
        player->UpdateRecord( kfstorerecord, kfsetting->_id, __STRING__( time ), KFEnum::Set, nexttime );

        // 刷新商品
        StoreRefreshGoods( player, kfsetting, kfstorerecord );

        // 下次刷新定时器
        StartRefreshStoreTimer( player, kfsetting->_id, nexttime );
    }

    __KF_RESET_FUNCTION__( KFStoreModule::OnResetRefreshStoreCount )
    {
        auto kfstorerecord = player->Find( __STRING__( store ) );
        for ( auto kfstore = kfstorerecord->First(); kfstore != nullptr; kfstore = kfstorerecord->Next() )
        {
            auto refreshcount = kfstore->Get<uint32>( __STRING__( refresh ) );
            if ( refreshcount == 0u )
            {
                continue;
            }

            auto kfsetting = KFStoreConfig::Instance()->FindSetting( kfstore->GetKeyID() );
            if ( kfsetting == nullptr )
            {
                continue;
            }

            for ( auto& costid : kfsetting->_refresh_cost_list )
            {
                auto kfcostsetting = KFCountCostConfig::Instance()->FindSetting( costid );
                if ( kfcostsetting->_refresh_time_id != timeid )
                {
                    continue;
                }

                player->UpdateObject( kfstore, __STRING__( refresh ), KFEnum::Set, 0u );
                break;
            }
        }
    }
}