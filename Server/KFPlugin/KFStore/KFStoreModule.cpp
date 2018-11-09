#include "KFStoreModule.h"
#include "KFStoreConfig.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
    KFStoreModule::KFStoreModule()
    {
        _kf_component = nullptr;
    }

    void KFStoreModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_store_config, true );
    }

    void KFStoreModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_BUY_STORE_REQ, &KFStoreModule::HandleBuyStoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_GIVE_STORE_REQ, &KFStoreModule::HandleGiveStoreReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_STORE_INFO_REQ, &KFStoreModule::HandleQueryStoreInfoReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SET_WISH_ORDER_REQ, &KFStoreModule::HandleSetWishOrderReq );
    }

    void KFStoreModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__( _kf_store_config );

        __UNREGISTER_MESSAGE__( KFMsg::MSG_BUY_STORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_GIVE_STORE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_STORE_INFO_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SET_WISH_ORDER_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleBuyStoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgBuyStoreReq );

        auto result = BuyStoreResult( player, kfmsg.buytype(), kfmsg.shopid(), kfmsg.num() );
        if ( KFMsg::StoreBuyOK == result )
        {
            KFMsg::MsgBuyStoreAck ack;
            ack.set_storeid( kfmsg.shopid() );
            ack.set_code( result );
            _kf_player->SendToClient( playerid, KFMsg::MSG_BUY_STORE_ACK, &ack );
        }
        else
        {
            _kf_display->SendToClient( player, result, kfmsg.shopid() );
        }
    }

    uint32 KFStoreModule::BuyStoreResult( KFEntity* player, const std::string& buytype, uint32 shopid, uint32 num )
    {
        auto storedatasetting = _kf_store_config->FindStoreSetting( shopid );
        if ( storedatasetting == nullptr )
        {
            return KFMsg::StoreParamError;
        }

        uint64 nowtime = KFGlobal::Instance()->_real_time;
        if ( !storedatasetting->CheckParam( buytype, KFStoreEnum::Buy, num, nowtime ) )
        {
            return KFMsg::StoreParamError;
        }

        //玩家最大拥有道具检测
        if ( storedatasetting->GetMaxOwns() &&
                !CheckOwnLimit( player, storedatasetting->_max_owm_, num, &storedatasetting->_buy_item ) )
        {
            return KFMsg::StoreOutOfLimitOwm;
        }

        auto kfagents = storedatasetting->FindBuyCostAgents( buytype, nowtime, num );
        if ( kfagents == nullptr )
        {
            return KFMsg::StoreParamError;
        }

        //检测扣除物品是否足够
        if ( !player->CheckAgentData( kfagents, __FUNC_LINE__ ) )
        {
            return KFMsg::StoreLackCost;
        }

        //限购
        if ( storedatasetting->CheckLimit() )
        {
            if ( !CheckLimitItem( player, storedatasetting->_id, num, storedatasetting->_start_buy_time, storedatasetting->_limit_count ) )
            {
                return KFMsg::StoreOutOfLimits;
            }
            if ( !storedatasetting->CheckOutOfData( nowtime ) )
            {
                return KFMsg::StoreOutOfLimitDate;
            }
            //记录限购状态
            SetLimitInfo( player, shopid, num, storedatasetting->_start_buy_time );
        }

        //扣除道具
        player->RemoveAgentData( kfagents, __FUNC_LINE__ );

        //发送道具
        player->AddAgentData( &storedatasetting->_buy_item, 1.0f, true, __FUNC_LINE__ );

        return KFMsg::StoreBuyOK;
    }

    bool KFStoreModule::CheckLimitItem( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime, uint32 buylimits )
    {
        auto kfobject = player->GetData();
        auto kfstore = kfobject->FindData( __KF_STRING__( store ), ( uint64 )shopid );
        if ( kfstore == nullptr )
        {
            return true;
        }

        //出现存储限购时间不一致 重置玩家的限购次数
        auto limittime = kfstore->GetValue<uint64>( __KF_STRING__( time ) );
        if ( startbuytime != limittime )
        {
            player->UpdateData( kfstore, shopid, __KF_STRING__( time ), KFOperateEnum::Set, startbuytime );
            player->UpdateData( kfstore, shopid, __KF_STRING__( count ), KFOperateEnum::Set, 0 );
            return true;
        }

        auto hadbuy = kfstore->GetValue<uint32>( __KF_STRING__( count ) );
        if ( hadbuy + num > buylimits )
        {
            return false;
        }
        return true;
    }


    ///FIXME:目前只判断玩家是否有物品，如果有返回false,不进行数量的判断
    bool KFStoreModule::CheckOwnLimit( KFEntity* player, uint32 maxowns, uint32 num, const KFAgents* kfagents )
    {
        return !player->CheckAgentData( kfagents, __FUNC_LINE__ );
    }

    void KFStoreModule::SetLimitInfo( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime )
    {
        auto kfobject = player->GetData();
        auto kfstorerecord = kfobject->FindData( __KF_STRING__( store ) );
        auto kfstore = kfobject->FindData( __KF_STRING__( store ), shopid );
        if ( nullptr == kfstore )
        {
            player->UpdateData( kfstorerecord, shopid, __KF_STRING__( id ), KFOperateEnum::Set, shopid );
            player->UpdateData( kfstorerecord, shopid, __KF_STRING__( time ), KFOperateEnum::Set, startbuytime );
            player->UpdateData( kfstorerecord, shopid, __KF_STRING__( count ), KFOperateEnum::Set, num );
            return;
        }
        player->UpdateData( kfstorerecord, shopid, __KF_STRING__( count ), KFOperateEnum::Add, num );
        return;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleGiveStoreReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgGiveStoreReq );

        auto kfobject = player->GetData();
        auto kffriend = kfobject->FindData( __KF_STRING__( friend ), kfmsg.toplayerid() );
        if ( kffriend == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::FriendNotExist );
        }

        auto kfsetting = _kf_store_config->FindStoreSetting( kfmsg.shopid() );
        if ( kfsetting == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreParamError );
        }

        uint64 nowtime = KFGlobal::Instance()->_real_time;
        if ( !kfsetting->CheckParam( kfsetting->_cost_give, KFStoreEnum::Give, 1, nowtime ) )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreOutOfLimits );
        }

        auto kfcostagents = kfsetting->FindGiveCostAgents( nowtime );
        if ( kfcostagents == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::StoreParamError );
        }

        if ( !player->CheckAgentData( kfcostagents, __FUNC_LINE__ ) )
        {
            return _kf_display->SendToClient( player, KFMsg::GiveLackCost );
        }

        // 发送邮件
        auto toserverid = kffriend->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
        auto ok = _kf_mail->SendMail( player, toserverid, kfmsg.toplayerid(), kfsetting->_give_mail_id, &kfsetting->_buy_item );
        if ( ok )
        {
            _kf_display->SendToClient( player, KFMsg::GiveBuyOK, kfmsg.shopid() );

            // 扣除消耗
            player->RemoveAgentData( kfcostagents, __FUNC_LINE__ );

            // 添加好友度
            _kf_relation->AddFriendLiness( player, kfmsg.toplayerid(), KFMsg::Give, kfsetting->_give_friend_liness );
        }
        else
        {
            _kf_display->SendToClient( player, KFMsg::MailServerBusy, kfmsg.shopid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleQueryStoreInfoReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryStoreInfoReq );

        // 版本号一致 不发商城详情
        KFMsg::MsgQueryStoreInfoAck ack;
        ack.set_version( _kf_store_config->GetVersion() );
        if ( kfmsg.version() != _kf_store_config->GetVersion() )
        {
            ack.CopyFrom( *( _kf_store_config->GetStoreClientShowInfo() ) );
        }

        _kf_player->SendToClient( player, KFMsg::MSG_QUERY_STORE_INFO_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFStoreModule::HandleSetWishOrderReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSetWishOrderReq );
        auto kfdata = player->GetData();
        auto kfwishorders = kfdata->FindData( __KF_STRING__( wishorder ) );

        uint32 result = KFMsg::StoreParamError;

        switch ( kfmsg.type() )
        {
        case KFMsg::StoreWishEnum::Add:
            result = AddWishOrder( player, kfwishorders, kfmsg.storeid(), kfmsg.status() );
            break;
        case KFMsg::StoreWishEnum::Del:
            result = RemoveWishOrder( player, kfwishorders, kfmsg.storeid() );
            break;
        case KFMsg::StoreWishEnum::Modify:
            result = ModifyWishOrder( player, kfwishorders, kfmsg.storeid(), kfmsg.status() );
            break;
        default:
            break;
        }
        _kf_display->SendToClient( player, result, kfmsg.storeid() );
    }


    uint32 KFStoreModule::AddWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status )
    {
        // 找不到商品
        if ( nullptr == kfwishorders || nullptr == _kf_store_config->FindStoreSetting( storeid ) )
        {
            return KFMsg::StoreParamError;
        }

        if ( KFMsg::WishStateEnum_MIN > status || KFMsg::WishStateEnum_MAX < status )
        {
            return KFMsg::StoreParamError;
        }

        // 已经在心愿单中
        if ( nullptr != kfwishorders->FindData( storeid ) )
        {
            return KFMsg::WishOrderHadStoreId;
        }

        // 超过心愿单最大限制
        static auto _wish_order_size = _kf_option->FindOption( __KF_STRING__( wishordermaxcount ) );
        if ( _wish_order_size->_uint32_value <= kfwishorders->Size() )
        {
            return KFMsg::WishOrderMaxSize;
        }

        auto kfwishorder = _kf_kernel->CreateObject( kfwishorders->GetDataSetting() );
        kfwishorder->SetValue< uint32 >( __KF_STRING__( id ), storeid );
        kfwishorder->SetValue< uint64 >( __KF_STRING__( time ), KFGlobal::Instance()->_real_time );
        kfwishorder->SetValue< uint32 >( __KF_STRING__( status ), status );
        player->AddData( kfwishorders, kfwishorder );
        return KFMsg::WishPanelAddSuccessed;
    }

    uint32 KFStoreModule::RemoveWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid )
    {
        if ( _invalid_int >= kfwishorders->Size() )
        {
            return KFMsg::WishOrderEmpty;
        }

        // 查找心愿单中是否有该物品
        if ( nullptr == kfwishorders->FindData( storeid ) )
        {
            return KFMsg::WishOrderNoExist;
        }

        player->RemoveData( kfwishorders, storeid );
        return KFMsg::Success;
    }

    uint32 KFStoreModule::ModifyWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status )
    {
        if ( KFMsg::WishStateEnum_MIN > status || KFMsg::WishStateEnum_MAX < status )
        {
            return KFMsg::StoreParamError;
        }

        // 查找心愿单中是否有该物品
        auto kfwishorder = kfwishorders->FindData( storeid );
        if ( nullptr == kfwishorder )
        {
            return KFMsg::WishOrderNoExist;
        }

        player->UpdateData( __KF_STRING__( wishorder ), storeid, __KF_STRING__( status ), KFOperateEnum::Set, status );
        return KFMsg::Success;
    }


}
