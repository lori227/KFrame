#include "KFPublicShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
//#define __USE_WORKER__
	
#ifdef __USE_WORKER__
#define __REGISTER_PUBLIC_MESSAGE__ __REGISTER_WORKER_MESSAGE__
#define __UNREGISTER_PUBLIC_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
#define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_public )
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
#define __REGISTER_PUBLIC_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_PUBLIC_MESSAGE__ __UNREGISTER_MESSAGE__
	static KFRedisDriver* _kf_redis_driver = nullptr;
#define __PUBLIC_REDIS_DRIVER__ _kf_redis_driver
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

	KFPublicShardModule::KFPublicShardModule()
	{

	}

	KFPublicShardModule::~KFPublicShardModule()
	{

	}

	void KFPublicShardModule::InitModule()
	{
	}

	void KFPublicShardModule::BeforeRun()
	{
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ, &KFPublicShardModule::HandleCreateRoleReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ, &KFPublicShardModule::HandleUpdatePublicDataReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ, &KFPublicShardModule::HandleQueryBasicDataReq );	
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ, &KFPublicShardModule::HandleSetPlayerNameReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_REQ, &KFPublicShardModule::HandlePlayerToastReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ, &KFPublicShardModule::HandleUpdateGuestListReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ, &KFPublicShardModule::HandleQueryGuestReq );
		__REGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_REQ, &KFPublicShardModule::HandleQueryToastCountReq );
	}
	
	void KFPublicShardModule::BeforeShut()
	{
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_PLAYER_TOAST_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ );
		__UNREGISTER_PUBLIC_MESSAGE__( KFMsg::S2S_QUERY_TOAST_COUNT_REQ );
		////////////////////////////////////////////////////////////////////////////////////////////////////////
		_kf_schedule->UnRegisterSchedule( this );
		////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	void KFPublicShardModule::OnceRun()
	{
#ifndef __USE_WORKER__
		// 初始化redis
		_kf_redis_driver = _kf_redis->CreateExecute( KFField::_public );
#endif

		auto cleartime = _kf_option->GetUInt32( "dailytoastcleartime" );

		auto kfsetting = _kf_schedule->CreateScheduleSetting();
		kfsetting->SetDate( KFScheduleEnum::Loop, 0, cleartime );
		kfsetting->SetData( __LINE__, nullptr, _invalid_int );
		_kf_schedule->RegisterSchedule( kfsetting, this, &KFPublicShardModule::OnScheduleCleanDailyToast );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdatePublicDataReq )
	{
		__PROTO_PARSE__( KFMsg::S2SUpdatePublicDataReq );

		MapString values;
		for ( auto i = 0; i < kfmsg.pbdata_size(); ++i )
		{
			auto pbdata = &kfmsg.pbdata( i );
			values[ pbdata->name() ] = pbdata->value();
		}

		auto redisdriver = __PUBLIC_REDIS_DRIVER__;
		redisdriver->VoidExecute( values, "hmset %s:%u", KFField::_public.c_str(), kfmsg.playerid() );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryBasicDataReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryBasicReq );
		auto redisdriver  = __PUBLIC_REDIS_DRIVER__;

		KFMsg::S2SQueryBasicAck ack;
		ack.set_playerid( kfmsg.playerid() );

		uint32 queryid = _invalid_int;
		redisdriver->UInt32Execute( queryid, "get %s:%s", KFField::_name.c_str(), kfmsg.name().c_str() );

		auto pbobject = ack.mutable_pbobject();
		pbobject->set_key( queryid );

		// 查询所有数据
		if ( queryid != _invalid_int )
		{
			MapString values;
			redisdriver->MapExecute( values, "hgetall %s:%u", KFField::_public.c_str(), queryid );
			for ( auto iter : values )
			{
				auto pbstring = pbobject->add_pbstring();
				pbstring->set_name( iter.first );
				pbstring->set_value( iter.second );
			}
		}

		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_BASIC_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleCreateRoleReq )
	{
		__PROTO_PARSE__( KFMsg::S2SCreateRoleReq );

		uint32 result = ProcessSetPlayerName( kfmsg.playerid(), _invalid_str, kfmsg.newname() );

		KFMsg::S2SCreateRoleAck ack;
		ack.set_playerid( kfmsg.playerid() );
		ack.set_newname( kfmsg.newname() );
		ack.set_modleid( kfmsg.modleid() );
		ack.set_result( result );
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_CREATE_ROLE_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleSetPlayerNameReq )
	{
		__PROTO_PARSE__( KFMsg::S2SSetPlayerNameReq );

		// 先查询名字
		uint32 result = ProcessSetPlayerName( kfmsg.playerid(), kfmsg.oldname(), kfmsg.newname() );

		KFMsg::S2SSetPlayerNameAck ack;
		ack.set_playerid( kfmsg.playerid() );
		ack.set_oldname( kfmsg.oldname() );
		ack.set_newname( kfmsg.newname() );
		ack.set_result( result );
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_SET_PLAYER_NAME_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandlePlayerToastReq )
	{
		__PROTO_PARSE__( KFMsg::S2SPlayerToastReq );
		auto redisdriver  = __PUBLIC_REDIS_DRIVER__;

		uint32 serverid = _invalid_int;
		redisdriver->UInt32Execute( serverid, "hget %s:%u %s", KFField::_public.c_str(), kfmsg.targetplayerid(), KFField::_server_id.c_str() );
	
		KFMsg::S2SPlayerToastAck ack;
		ack.set_playerid( kfmsg.selfplayerid() );
		ack.set_targetplayerid( kfmsg.targetplayerid() );
		ack.set_targetserverid( serverid );

		//获取玩家今日的被敬酒次数
		uint32 toastcount = _invalid_int;
		redisdriver->UInt32Execute( toastcount, "hget %s %u", KFField::_toast.c_str(), kfmsg.targetplayerid() );
	
		//判断被敬酒次数是否用完
		if ( toastcount >= kfmsg.dailygetlimit() )
		{
			ack.set_result( KFMsg::ToastGetCountOver );
			__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_PLAYER_TOAST_ACK, &ack );
			return;
		}

		// 增加玩家今日被敬酒次数和总的被敬酒次数
		redisdriver->AppendCommand( "hincrby %s %u %u", KFField::_toast.c_str(), kfmsg.targetplayerid(), 1 );
		redisdriver->AppendCommand( "hincrby %s %u %u", KFField::_toast_count.c_str(), kfmsg.targetplayerid(), 1 );
		auto ok = redisdriver->PipelineExecute();
		if ( !ok )
		{
			ack.set_result( KFMsg::ToastFailed );
			__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_PLAYER_TOAST_ACK, &ack );
			return;
		}

		ack.set_result( KFMsg::ToastOK );
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_PLAYER_TOAST_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdateGuestListReq )
	{
		__PROTO_PARSE__( KFMsg::S2SUpdateGuestListReq );

		// 保存访客信息
		auto redisdriver = __PUBLIC_REDIS_DRIVER__;
		redisdriver->VoidExecute( "zadd %s:%u %u %u", KFField::_guest.c_str(), kfmsg.playerid(), kfmsg.guesttime(), kfmsg.guestid() );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryGuestReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryGuestReq );
		auto redisdriver = __PUBLIC_REDIS_DRIVER__;
		
		// 删除过期访客信息
		auto querytime = kfmsg.querytime();
		uint64 validtime = TimeEnum::OneDaySecond * TimeEnum::OneMonthDay;
		if ( querytime > validtime )
		{
			redisdriver->VoidExecute( "zremrangebyscore %s:%u %u %u", KFField::_guest.c_str(), kfmsg.queryid(), _invalid_int, querytime-validtime);
		}
		else
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] [%u] querytime error!",
				__FUNCTION__, kfmsg.querytime() );
		}

		// 返回访客信息
		KFMsg::S2SQueryGuestAck ack;
		ack.set_playerid( kfmsg.playerid() );
		ack.set_queryid( kfmsg.queryid() );

		// 获取访客数量
		uint64 querycount = _invalid_int;
		redisdriver->UInt64Execute( querycount, "zcard %s:%u", KFField::_guest.c_str(), kfmsg.queryid() );
		ack.set_guestcount( querycount );

		// 获取最近四位访客的信息
		if ( querycount != _invalid_int )
		{
			VectorString values;
			redisdriver->VectorExecute( values, "zrevrange %s:%u 0 3", KFField::_guest.c_str(), kfmsg.queryid() );
			for ( auto& strid : values )
			{
				auto guestid = KFUtility::ToValue< uint32 >( strid );
				std::string guesticon;
				redisdriver->StringExecute( guesticon, "hget %s:%u %s", KFField::_public.c_str(), guestid, KFField::_icon.c_str() );
				std::string guesticonbox;
				redisdriver->StringExecute( guesticonbox, "hget %s:%u %s", KFField::_public.c_str(), guestid, KFField::_icon_box.c_str() );
				auto guestdata = ack.add_guestdata();
				guestdata->set_guestid( guestid );
				guestdata->set_guesticon( guesticon );
				guestdata->set_guesticonbox( guesticonbox );
			}
		}
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_GUEST_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryToastCountReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryToastCountReq );
		auto redisdriver = __PUBLIC_REDIS_DRIVER__;

		uint32 toastcount = _invalid_int;
		redisdriver->UInt32Execute( toastcount, "hget %s %u", KFField::_toast_count.c_str(), kfmsg.targetplayerid() );

		KFMsg::S2SQueryToastCountAck ack;
		ack.set_playerid( kfmsg.selfplayerid() );
		ack.set_targetplayerid( kfmsg.targetplayerid() );
		ack.set_toastcount( toastcount );
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_TOAST_COUNT_ACK, &ack );
	}

	uint32 KFPublicShardModule::ProcessSetPlayerName( uint32 playerid, const std::string& oldname, const std::string& newname )
	{
		auto redisdriver  = __PUBLIC_REDIS_DRIVER__;

		uint32 queryplayerid = 0;
		if ( !redisdriver->UInt32Execute( queryplayerid, "get %s:%s", KFField::_name.c_str(), newname.c_str() ) )
		{
			return KFMsg::PublicDatabaseError;
		}

		// 如果已经设置, 并且不是自己, 返回错误
		if ( queryplayerid != 0 && queryplayerid != playerid )
		{
			return KFMsg::NameAlreadyExist;
		}

		// 保存名字
		if ( !redisdriver->VoidExecute( "set %s:%s %u", KFField::_name.c_str(), newname.c_str(), playerid ) )
		{
			return KFMsg::PublicDatabaseError;
		}

		// 删除旧的名字关联
		if ( !oldname.empty() )
		{
			redisdriver->VoidExecute( "del %s:%s", KFField::_name.c_str(), oldname.c_str() );
		}

		return KFMsg::Success;
	}

	void KFPublicShardModule::OnScheduleCleanDailyToast( uint32 id, const char * data, uint32 size )
	{
		//清除所有玩家的被敬酒次数
		auto redisdriver = __PUBLIC_REDIS_DRIVER__;
		redisdriver->VoidExecute( "del %s", KFField::_toast.c_str() );
	}
}