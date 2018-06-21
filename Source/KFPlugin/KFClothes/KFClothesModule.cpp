#include "KFClothesModule.h"
#include "KFClothesConfig.h"
#include "KFProtocol/KFProtocol.h"


namespace KFrame
{
	KFClothesModule::KFClothesModule()
	{
		_kf_component = nullptr;
	}

	KFClothesModule::~KFClothesModule()
	{
	}

	void KFClothesModule::InitModule()
	{
		///////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_clothes_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}

	void KFClothesModule::BeforeRun()
	{
		_kf_component = _kf_kernel->FindComponent( KFField::_player );
		_kf_component->RegisterAddDataFunction( KFField::_clothes, this, &KFClothesModule::OnAddClothesCallBack );
		_kf_component->RegisterRemoveDataFunction( KFField::_clothes, this, &KFClothesModule::OnRemoveClothesCallBack );
		_kf_component->RegisterUpdateDataFunction( KFField::_clothes, KFField::_count, this, &KFClothesModule::OnClothesCountUpdateCallBack );
		//////////////////////////////////////////////////////////////////////////////////////////////////

		_kf_component->RegisterAddAgentFunction( KFField::_clothes, this, &KFClothesModule::AddClothesAgentData );
		//////////////////////////////////////////////////////////////////////////////////////////////////

		_kf_player->RegisterEnterFunction( this, &KFClothesModule::OnEnterStartClothesTimer );
		_kf_player->RegisterLeaveFunction( this, &KFClothesModule::OnLeaveStopClothesTimer );

		//////////////////////////////////////////////////////////////////////////////////////////////////
		__REGISTER_MESSAGE__( KFMsg::MSG_DRESS_CLOTHES_REQ, &KFClothesModule::HandleDressClothesReq );
		__REGISTER_MESSAGE__( KFMsg::MSG_SET_MODEL_DEFAULT_CLOTHES_REQ, &KFClothesModule::HandleSetModelDefaultClothesReq );
		__REGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MODEL_REQ, &KFClothesModule::HandleChangeModelReq );
	}

	void KFClothesModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );

		_kf_component->UnRegisterAddDataFunction( KFField::_clothes );
		_kf_component->UnRegisterRemoveDataFunction( KFField::_clothes );
		_kf_component->UnRegisterUpdateDataFunction( KFField::_clothes, KFField::_count );

		_kf_component->UnRegisterAddAgentFunction( KFField::_clothes );
		//////////////////////////////////////////////////////////////////////////////////////////////////

		_kf_player->UnRegisterEnterFunction( this );
		_kf_player->UnRegisterLeaveFunction( this );
		//////////////////////////////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MESSAGE__( KFMsg::MSG_DRESS_CLOTHES_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::MSG_SET_MODEL_DEFAULT_CLOTHES_REQ );
		__UNREGISTER_MESSAGE__( KFMsg::MSG_CHANGE_MODEL_REQ );
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KFClothesModule::OnEnterStartClothesTimer( KFEntity* player )
	{
		CheckStartClothesTimer( player );
	}

	void KFClothesModule::OnLeaveStopClothesTimer( KFEntity* player )
	{
		_kf_timer->UnRegisterTimer( this, player->GetKeyID() );
	}
		
	void KFClothesModule::CheckStartClothesTimer( KFEntity* player )
	{
		auto kfobject = player->GetData();
		auto kfclothesrecord = kfobject->FindData( KFField::_clothes );

		// 检查所有衣服, 找到时间最少的一个衣服
		uint32 _min_valid_time = 0xFFFFFFFF;

		auto kfclothes = kfclothesrecord->FirstData();
		while ( kfclothes != nullptr )
		{
			auto validtime = kfclothes->GetValue( KFField::_time );
			if ( validtime != 0 && validtime < _min_valid_time )
			{
				_min_valid_time = validtime;
			}

			kfclothes = kfclothesrecord->NextData();
		}

		if ( _min_valid_time == 0xFFFFFFFF )
		{
			return;
		}

		// 如果已经有超时时装
		if ( _min_valid_time < KFGlobal::Instance()->_real_time )
		{
			_min_valid_time = KFGlobal::Instance()->_real_time;
		}

		// 转换成间隔时间(毫秒) 启动定时器
		auto intervaltime = ( _min_valid_time - KFGlobal::Instance()->_real_time + 1 ) * 1000;
		_kf_timer->RegisterLimitTimer( player->GetKeyID(), intervaltime, 1, this, &KFClothesModule::OnTimerCheckClothesValidTime );
	}

	void KFClothesModule::RemoveValidTimeoutClothes( KFEntity* player )
	{
		auto kfobject = player->GetData();
		auto kfclothesrecord = kfobject->FindData( KFField::_clothes );
		if ( kfclothesrecord == nullptr )
		{
			return;
		}

		// 查找时间道具
		std::map< KFData*, uint64 > timeclothes;
		auto kfclothes = kfclothesrecord->FirstData();
		while ( kfclothes != nullptr )
		{
			auto validtime = kfclothes->GetValue( KFField::_time );
			if ( validtime != 0 )
			{
				timeclothes.insert( std::make_pair( kfclothes, validtime ) );
			}

			kfclothes = kfclothesrecord->NextData();
		}

		// 删除时间道具
		for ( auto& iter : timeclothes )
		{
			auto* kfclothes = iter.first;
			if ( KFGlobal::Instance()->_real_time < iter.second )
			{
				continue;
			}

			// 删除道具
			auto keyid = kfclothes->GetKeyID();
			player->RemoveData( kfclothesrecord, keyid );
		}
	}

	__KF_TIMER_FUNCTION__( KFClothesModule::OnTimerCheckClothesValidTime )
	{
		auto player = _kf_player->FindPlayer( objectid, __FUNCTION_LINE__ );
		if ( player == nullptr )
		{
			return;
		}

		// 删除过期的时装
		RemoveValidTimeoutClothes( player );

		// 检查是否需要开启定时器
		CheckStartClothesTimer( player );
	}
	
	void KFClothesModule::AddClothesAgentData( KFEntity* player, KFAgent* kfagent, float multiple, const char* function, uint32 line )
	{
		auto configid = kfagent->_config_id;
		if ( configid == _invalid_int )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] [%s] clothes id = 0!",
				function, line, kfagent->_string.c_str() );
		}

		auto kfsetting = _kf_clothes_config->FindClothesSetting( configid );
		if ( kfsetting == nullptr )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] [%s] clothes setting = null!",
				function, line, kfagent->_string.c_str() );
		}

		auto kfagengvalue = kfagent->FindAgentValue( KFField::_count );
		if ( kfagengvalue == nullptr )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] [%s] clothes count = null!",
				function, line, kfagent->_string.c_str() );
		}

		auto clothescount = _kf_kernel->CalcAgentValue( kfagengvalue, multiple );

		auto kfobject = player->GetData();
		auto kfclothesrecord = kfobject->FindData( KFField::_clothes );
		if ( kfclothesrecord == nullptr )
		{
			return KFLogger::LogSystem( KFLogger::Error, "[%s:%u] [%s] player[%s] clothes record = null!", 
				function, line, kfagent->_string.c_str(), player->GetKeyString() );
		}
		auto datasetting = kfclothesrecord->GetDataSetting();
	
		// 判断是否存在该时装
		auto kfclothes = kfclothesrecord->FindData( configid );
		if ( kfclothes == nullptr )
		{
			// 不存在, 直接加
			kfclothes = _kf_kernel->CreateObject( kfclothesrecord->GetDataSetting() );

			// 设置属性
			for ( auto iter : kfagent->_datas._objects )
			{
				auto name = iter.first;
				auto value = _kf_kernel->CalcAgentValue( iter.second, multiple );
				if ( name == KFField::_time )
				{
					value = KFGlobal::Instance()->_real_time + value;
				}

				kfclothes->SetValue( name, value );
			}

			// 添加物品
			player->AddData( kfclothesrecord, configid, kfclothes );
		}
		else
		{
			// 存在, 判断有效时间
			auto agenttime = kfagent->GetValue( KFField::_time );
			auto validtime = kfclothes->GetValue< uint64 >( KFField::_time );
			if ( validtime == 0 )
			{
				if ( agenttime == 0 )
				{
					// 永久时装, 更新数量
					player->UpdateData( kfclothes, KFField::_count, KFOperateEnum::Add, clothescount );
				}
				else
				{
					// 时限时装, 转换成金币
					auto multiple = agenttime / TimeEnum::OneDaySecond;
					if ( multiple != 0 )
					{
						player->AddAgentData( &kfsetting->_exchange, multiple, true, __FUNCTION_LINE__ );
					}
				}
			}
			else
			{
				if ( agenttime == 0 )
				{
					// 永久时装, 直接把时间更新成永久
					player->UpdateData( kfclothes, KFField::_time, KFOperateEnum::Set, 0 );
					player->UpdateData( kfclothes, KFField::_count, KFOperateEnum::Set, clothescount );
				}
				else
				{
					// 时限时装
					player->UpdateData( kfclothes, KFField::_time, KFOperateEnum::Add, agenttime );
				}
			}
		}
	}
	
	void KFClothesModule::OnClothesCountUpdateCallBack( KFEntity* player, uint64 key, KFData* kfdata, uint32 operate, uint64 value, uint64 oldvalue, uint64 newvalue )
	{
		// 调用脚本
		if ( newvalue > 0 )
		{
			return;
		}

		// 数量为0, 删除该时装
		player->RemoveData( KFField::_clothes, key );
	}

	void KFClothesModule::OnAddClothesCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )
	{
		auto validtime = kfdata->GetValue< uint64 >( KFField::_time );
		if ( validtime == 0 )
		{
			return;
		}

		// 时间道具 开启有效时间
		CheckStartClothesTimer( player );
	}

	void KFClothesModule::OnRemoveClothesCallBack( KFEntity* player, KFData* kfparent, uint64 key, KFData* kfdata )
	{
		auto kfsetting = _kf_clothes_config->FindClothesSetting( static_cast< uint32 >( key ) );
		if ( kfsetting == nullptr )
		{
			return;
		}
		auto kfobject = player->GetData();
		if ( kfobject == nullptr )
		{
			return;
		}
		auto kfmodelrecord = kfobject->FindData( KFField::_model );
		if ( kfmodelrecord == nullptr )
		{
			return;
		}

		//判断被删除时装是否是默认时装
		auto kfmodel = kfmodelrecord->FirstData();
		while ( kfmodel != nullptr )
		{
			auto clothesid = kfmodel->GetValue< uint32 >( KFField::_clothes_id );
			if ( clothesid != _invalid_int && clothesid == key )
			{
				player->UpdateData( kfmodel, KFField::_clothes_id, KFOperateEnum::Set, _invalid_int );
			}

			kfmodel = kfmodelrecord->NextData();
		}

		//判断被删除时装是否是当前穿戴时装
		auto kfclothesid = kfobject->GetValue< uint32 >( KFField::_clothes_id );
		if ( kfclothesid != _invalid_int && kfclothesid == key )
		{
			player->UpdateData( KFField::_clothes_id, KFOperateEnum::Set, _invalid_int );
		}

		// 发送邮件
		_kf_mail->SendMail( player, kfsetting->_mail_id, nullptr );
	}

	__KF_MESSAGE_FUNCTION__( KFClothesModule::HandleDressClothesReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgDressClothesReq );

		if ( _invalid_int != kfmsg.clothesid() )
		{
			auto kfobject = player->GetData();
			auto kfclothes = kfobject->FindData( KFField::_clothes, kfmsg.clothesid() );
			if ( kfclothes == nullptr )
			{
				return _kf_display->SendToClient( player, KFMsg::ClothesNotExist );
			}

			// 判断和自己模型是否匹配
			auto kfsetting = _kf_clothes_config->FindClothesSetting( kfmsg.clothesid() );
			if ( kfsetting == nullptr )
			{
				return _kf_display->SendToClient( player, KFMsg::ClothesDataError, kfmsg.clothesid() );
			}

			auto modleid = kfobject->GetValue< uint32 >( KFField::_model_id );
			if ( !kfsetting->HasModle( modleid ) )
			{
				return _kf_display->SendToClient( player, KFMsg::ClothesModleNotMatch );
			}
		}

		player->UpdateData( KFField::_clothes_id, KFOperateEnum::Set, kfmsg.clothesid() );
		_kf_display->SendToClient( player, KFMsg::ClothesDressOK );
	}

	__KF_MESSAGE_FUNCTION__( KFClothesModule::HandleSetModelDefaultClothesReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgSetModelDefaultClothesReq );
		
		// 玩家模型信息
		auto kfobject = player->GetData();
		auto kfmodle = kfobject->FindData( KFField::_model, kfmsg.modelid() );
		if ( kfmodle == nullptr )
		{
			return _kf_display->SendToClient( player, KFMsg::ModuleIdNotFind );
		}

		// 判断是否有这个时装
		if ( kfmsg.clothesid() != _invalid_int )
		{
			auto kfclothes = kfobject->FindData( KFField::_clothes, kfmsg.clothesid() );
			if ( kfclothes == nullptr )
			{
				return _kf_display->SendToClient( player, KFMsg::ClothesNotExist );
			}
		}

		_kf_display->SendToClient( player, KFMsg::SetModelClothesOK );
		player->UpdateData( kfmodle, KFField::_clothes_id, KFOperateEnum::Set, kfmsg.clothesid() );
	}

	__KF_MESSAGE_FUNCTION__( KFClothesModule::HandleChangeModelReq )
	{
		__CLIENT_PROTO_PARSE__( KFMsg::MsgChangeModelReq );

		// 判断存在模型
		auto kfobject = player->GetData();
		auto kfmodel = kfobject->FindData( KFField::_model, kfmsg.modelid() );
		if ( kfmodel == nullptr )
		{
			return _kf_display->SendToClient( player, KFMsg::ModelNotExist );
		}

		player->UpdateData( KFField::_model_id, KFOperateEnum::Set, kfmsg.modelid() );

		// 判断默认时装
		auto clothesid = kfmodel->GetValue< uint32 >( KFField::_clothes_id );
		player->UpdateData( KFField::_clothes_id, KFOperateEnum::Set, clothesid );

		_kf_display->SendToClient( player, KFMsg::ModelChangeOK );
	}
}