#include "KFStoreModule.h"
#include "KFStoreConfig.h"
//#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
	KFStoreModule::KFStoreModule()
	{
		_kf_component = nullptr;
	}

	KFStoreModule::~KFStoreModule()
	{
	}

	void KFStoreModule::InitModule()
	{
		_kf_config->AddConfig( _kf_store_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
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
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
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
			_kf_player->SendMessageToClient( playerid, KFMsg::MSG_BUY_STORE_ACK, &ack );
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
		if ( !player->CheckAgentData( kfagents, __FUNCTION_LINE__ ) )
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
		player->RemoveAgentData( kfagents, __FUNCTION_LINE__ );

		//发送道具
		player->AddAgentData( &storedatasetting->_buy_item, 1.0f, true, __FUNCTION_LINE__ );

		return KFMsg::StoreBuyOK;
	}

	bool KFStoreModule::CheckLimitItem( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime, uint32 buylimits )
	{
		auto kfobject = player->GetData();
		auto kfstore = kfobject->FindData( KFField::_store, ( uint64 )shopid );
		if ( kfstore == nullptr )
		{
			return true;
		}

		//出现存储限购时间不一致 重置玩家的限购次数
		auto limittime = kfstore->GetValue<uint64>( KFField::_time );
		if ( startbuytime != limittime )
		{
			player->UpdateData( KFField::_store, shopid, KFField::_time, KFOperateEnum::Set, startbuytime );
			player->UpdateData( KFField::_store, shopid, KFField::_count, KFOperateEnum::Set, 0 );
			return true;
		}

		auto hadbuy = kfstore->GetValue<uint32>( KFField::_count );
		if ( hadbuy + num > buylimits )
		{
			return false;
		}
		return true;
	}


	///FIXME:目前只判断玩家是否有物品，如果有返回false,不进行数量的判断
	bool KFStoreModule::CheckOwnLimit( KFEntity* player, uint32 maxowns, uint32 num, const KFAgents* kfagents )
	{
		return !player->CheckAgentData( kfagents, __FUNCTION_LINE__ );
	}

	void KFStoreModule::SetLimitInfo( KFEntity* player, uint32 shopid, uint32 num, uint64 startbuytime )
	{
		auto kfobject = player->GetData();
		auto kfstorerecord = kfobject->FindData( KFField::_store );
		auto kfstore = kfobject->FindData( KFField::_store, ( uint64 )shopid );
		if ( nullptr == kfstore )
		{
			player->UpdateData( kfstorerecord, shopid, KFField::_id, KFOperateEnum::Set, shopid );
			player->UpdateData( KFField::_store, shopid, KFField::_time, KFOperateEnum::Set, startbuytime );
			player->UpdateData( KFField::_store, shopid, KFField::_count, KFOperateEnum::Set, num );
			return;
		}
		player->UpdateData( KFField::_store, shopid, KFField::_count, KFOperateEnum::Add, num );
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	__KF_MESSAGE_FUNCTION__( KFStoreModule::HandleGiveStoreReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgGiveStoreReq );

		auto kfobject = player->GetData();
		auto kffriend = kfobject->FindData( KFField::_friend, kfmsg.toplayerid() );
		if ( kffriend == nullptr )
		{
			return _kf_display->SendToClient( player, KFMsg::FriendNotExist );
		}

		auto toserverid = kffriend->GetValue<uint32>( KFField::_basic, KFField::_server_id );
		auto result = GiveStoreResult( player, kfmsg.shopid(), kfmsg.toplayerid(), toserverid );
		_kf_display->SendToClient( player, result, kfmsg.shopid() );
	}

	uint32 KFStoreModule::GiveStoreResult( KFEntity* player, uint32 shopid, uint32 toplayid, uint32 toserverid )
	{
		auto storedatasetting = _kf_store_config->FindStoreSetting( shopid );
		if ( storedatasetting == nullptr )
		{
			return KFMsg::StoreParamError;
		}

		uint64 nowtime = KFGlobal::Instance()->_real_time;
		if ( !storedatasetting->CheckParam( storedatasetting->_cost_give, KFStoreEnum::Give, 1, nowtime ) )
		{
			return KFMsg::StoreParamError;
		}

		//检测扣除物品是否足够
		auto kfcostagents = storedatasetting->FindGiveCostAgents( nowtime );
		if ( kfcostagents == nullptr )
		{
			return KFMsg::StoreParamError;
		}

		if ( !player->CheckAgentData( kfcostagents, __FUNCTION_LINE__ ) )
		{
			return KFMsg::GiveLackCost;
		}

		player->RemoveAgentData( kfcostagents, __FUNCTION_LINE__ );

		// 发送邮件
		_kf_mail->SendMail( player, KFGuid( toserverid, toplayid ), storedatasetting->_give_mail_id, &storedatasetting->_buy_item );
		return KFMsg::GiveBuyOK;
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

		_kf_player->SendMessageToClient( player, KFMsg::MSG_QUERY_STORE_INFO_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFStoreModule::HandleSetWishOrderReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgSetWishOrderReq );
		auto kfdata = player->GetData();
		auto kfwishorders = kfdata->FindData( KFField::_wish_order );

		uint32 result = KFMsg::AckEnum::StoreParamError;

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
		//_kf_player->SendResultAck( gateid, playerid, KFMsg::ModuleEnum::Store, result );
	}


	uint32 KFStoreModule::AddWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status )
	{
		// 找不到商品
		if ( nullptr == kfwishorders || nullptr == _kf_store_config->FindStoreSetting( storeid ) )
		{
			return KFMsg::AckEnum::StoreParamError;
		}

		if ( KFMsg::WishStateEnum_MIN > status || KFMsg::WishStateEnum_MAX < status )
		{
			return KFMsg::AckEnum::StoreParamError;
		}

		// 已经在心愿单中
		if ( nullptr != kfwishorders->FindData( storeid ) )
		{
			return KFMsg::AckEnum::WishOrderHadStoreId;
		}

		// 超过心愿单最大限制
		if ( _kf_option->GetUInt32( "wishordersize" ) <= kfwishorders->Size() )
		{
			return KFMsg::AckEnum::WishOrderMaxSize;
		}

		auto kfwishorder = _kf_kernel->CreateObject( kfwishorders->GetDataSetting() );
		kfwishorder->SetValue< uint32 >( KFField::_id, storeid );
		kfwishorder->SetValue< uint64 >( KFField::_time, KFGlobal::Instance()->_real_time );
		kfwishorder->SetValue< uint32 >( KFField::_status, status );
		player->AddData( kfwishorders, kfwishorder );
		return KFMsg::AckEnum::WishPanelAddSuccessed;
	}

	uint32 KFStoreModule::RemoveWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid )
	{
		if ( _invalid_int >= kfwishorders->Size() )
		{
			return KFMsg::AckEnum::WishOrderEmpty;
		}

		// 查找心愿单中是否有该物品
		if ( nullptr == kfwishorders->FindData( storeid ) )
		{
			return KFMsg::AckEnum::WishOrderNoExist;
		}

		player->RemoveData( kfwishorders, storeid );
		return KFMsg::AckEnum::Success;
	}

	uint32 KFStoreModule::ModifyWishOrder( KFEntity* player, KFData* kfwishorders, uint32 storeid, uint32 status )
	{
		if ( KFMsg::WishStateEnum_MIN > status || KFMsg::WishStateEnum_MAX < status )
		{
			return KFMsg::AckEnum::StoreParamError;
		}

		// 查找心愿单中是否有该物品
		auto kfwishorder = kfwishorders->FindData( storeid );
		if ( nullptr == kfwishorder )
		{
			return KFMsg::AckEnum::WishOrderNoExist;
		}

		player->UpdateData( KFField::_wish_order, storeid, KFField::_status, KFOperateEnum::Set, status );
		return KFMsg::AckEnum::Success;
	}


}
