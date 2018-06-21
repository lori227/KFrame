#include "KFMailShardModule.h"

namespace KFrame
{
//#define __USE_WORKER__

#ifdef __USE_WORKER__
#define __REGISTER_MAIL_MESSAGE__ __REGISTER_WORKER_MESSAGE__
#define __UNREGISTER_MAIL_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
#define __MAIL_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_mail )
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
#define __REGISTER_MAIL_MESSAGE__ __REGISTER_MESSAGE__
#define __UNREGISTER_MAIL_MESSAGE__ __UNREGISTER_MESSAGE__
	static KFRedisDriver* _kf_redis_driver = nullptr;
#define __MAIL_REDIS_DRIVER__ _kf_redis_driver
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

	KFMailShardModule::KFMailShardModule()
	{

	}

	KFMailShardModule::~KFMailShardModule()
	{

	}

	void KFMailShardModule::InitModule()
	{

	}

	void KFMailShardModule::BeforeRun()
	{
		__REGISTER_MAIL_MESSAGE__( KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
		__REGISTER_MAIL_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
		__REGISTER_MAIL_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ, &KFMailShardModule::HandleDeleteMailReq );
		__REGISTER_MAIL_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ, &KFMailShardModule::HandleUpdateMailFlagReq );
		__REGISTER_MAIL_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ, &KFMailShardModule::HandleNewPlayerLoginMailReq );
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	void KFMailShardModule::BeforeShut()
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////
		__UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ );
		__UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
		__UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ );
		__UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ );
		__UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ );
	}

	void KFMailShardModule::OnceRun()
	{
#ifndef __USE_WORKER__
		// 初始化redis
		_kf_redis_driver = _kf_redis->CreateExecute( KFField::_mail );
#endif

		auto cleartime = _kf_option->GetUInt32( "wholemailcleartime" );

		// 每天5点 清理过期的全局邮件
		auto kfsetting = _kf_schedule->CreateScheduleSetting();
		kfsetting->SetDate( KFScheduleEnum::Loop, 0, cleartime );
		_kf_schedule->RegisterSchedule( kfsetting, this, &KFMailShardModule::OnScheduleClearWholeOverdueMail );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void KFMailShardModule::OnScheduleClearWholeOverdueMail( uint32 id, const char* data, uint32 size )
	{
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		VectorString querymaillist;
		redisdriver->VectorExecute( querymaillist, "zrangebyscore %s -inf +inf", KFField::_whole_mail.c_str() );
		if ( querymaillist.empty() )
		{
			return;
		}

		VectorString overduelist;
		for ( auto& strmailid : querymaillist )
		{
			std::string queryid = _invalid_str;
			auto ok = redisdriver->StringExecute( queryid, "hget %s:%s %s", KFField::_mail.c_str(), strmailid.c_str(), KFField::_id.c_str() );
			if ( !ok )
			{
				continue;
			}

			if ( queryid == _invalid_str )
			{
				overduelist.push_back( strmailid );
			}
		}

		if ( !overduelist.empty() )
		{
			redisdriver->VoidExecute( overduelist, "zrem %s", KFField::_whole_mail.c_str() );
		}
	}

	std::string KFMailShardModule::FormatMailKeyName( uint32 playerid, uint32 mailtype, const char* function, uint32 line )
	{
		switch ( mailtype )
		{
		case  KFMsg::MailEnum::WholeMail:
			return KFUtility::Format( "%s:%u", KFField::_whole_mail.c_str(), playerid );
		case KFMsg::MailEnum::FriendMail:
			return KFUtility::Format( "%s:%u", KFField::_person_mail_friend.c_str(), playerid );
		case KFMsg::MailEnum::GiftMail:
			return KFUtility::Format( "%s:%u", KFField::_person_mail_gift.c_str(), playerid );
		default:
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u] mailtype[%u] error!",
				function, line, playerid, mailtype );
			break;
		}

		return _invalid_str;
	}

	__KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleQueryMailReq )
	{
		__PROTO_PARSE__( KFMsg::S2SQueryMailReq );

		// 计算玩家的全局邮件信息
		if ( kfmsg.mailtype() == KFMsg::MailEnum::WholeMail )
		{
			LoadWholeMailToPerson( kfmsg.playerid() );
		}

		// 查询邮件列表
		auto maillistkey = FormatMailKeyName( kfmsg.playerid(), kfmsg.mailtype(), __FUNCTION_LINE__ );
		if ( maillistkey.empty() )
		{
			return;
		}

		auto redisdriver = __MAIL_REDIS_DRIVER__;

		LesserMapString querymaillist;
		redisdriver->MapExecute( querymaillist, "zrange %s 0 -1 WITHSCORES", maillistkey.c_str() );
		if ( querymaillist.empty() )
		{
			return;
		}

		auto strmaxmailid = __KF_STRING__( kfmsg.maxmailid() );
		auto itermail = querymaillist.find( strmaxmailid );
		if (_invalid_int == kfmsg.maxmailid() )
		{
			itermail = querymaillist.end();
		}


		KFMsg::S2SQueryMailAck ack;
		ack.set_playerid( kfmsg.playerid() );
		auto pbmails = ack.mutable_mails();
		ack.set_mailtype( kfmsg.mailtype() );

		VectorString overduelist;
		auto maxmailcount = _kf_option->GetUInt32( "maxmailcount", kfmsg.mailtype() );
		auto iterbegin = querymaillist.begin();
		for ( ;iterbegin != itermail; ++iterbegin )
		{
			auto& strmailid = iterbegin->first;

			MapString maildata;
			auto ok = redisdriver->MapExecute( maildata, "hgetall %s:%s", KFField::_mail.c_str(), strmailid.c_str() );
			if ( !ok )
			{
				continue;
			}

			// 已经过期了
			if ( maildata.empty() )
			{
				overduelist.push_back( strmailid );
				continue;
			}

			auto pbmail = pbmails->add_mail();
			for ( auto& iter : maildata )
			{
				auto pbdata = pbmail->add_data();
				pbdata->set_name( iter.first );

				// 系统邮件做特殊处理
				if ( iter.first == KFField::_flag &&
					kfmsg.mailtype() == KFMsg::MailEnum::WholeMail )
				{
					pbdata->set_value( iterbegin->second );
				}
				else
				{
					pbdata->set_value( iter.second );
				}
			}

			if ( static_cast< uint32 >( pbmails->mail_size() ) >= maxmailcount )
			{
				break;
			}
		}

		if ( pbmails->mail_size() != 0 )
		{
			__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_MAIL_ACK, &ack );
		}

		// 删除过期邮件
		if ( !overduelist.empty() )
		{
			redisdriver->VoidExecute( overduelist, "zrem %s", maillistkey.c_str() );
		}
	}

	__KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
	{
		__PROTO_PARSE__( KFMsg::S2SAddMailReq );

		auto maillistkey = FormatMailKeyName( kfmsg.playerid(), kfmsg.mailtype(), __FUNCTION_LINE__ );
		if ( maillistkey.empty() )
		{
			return;
		}

		MapString maildata;
		auto pbmail = &kfmsg.pbmail();
		for ( auto i = 0; i < pbmail->data_size(); ++i )
		{
			auto pbdata = &pbmail->data( i );
			maildata[ pbdata->name() ] = pbdata->value();
		}

		// 添加邮件
		auto ok = AddMail( maillistkey, maildata );
		if ( !ok )
		{
			auto strmaildata = kfmsg.DebugString();
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u] add mail[%s] failed!",
				__FUNCTION_LINE__, kfmsg.playerid(), strmaildata.c_str() );
		}
	}

	bool KFMailShardModule::AddMail( const std::string& maillistkey, MapString& maildata )
	{
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		// 创建一个邮件id
		uint64 mailid = _invalid_int;
		redisdriver->UInt64Execute( mailid, "incr %s", KFField::_mail_id_creater.c_str() );
		if ( mailid == _invalid_int )
		{
			return false;
		}

		auto strmailid = __KF_STRING__( mailid );
		maildata[ KFField::_id ] = strmailid;
		maildata[ KFField::_send_time ] = __KF_STRING__( KFGlobal::Instance()->_real_time );

		// 添加邮件
		redisdriver->AppendCommand( maildata, "hmset %s:%s", KFField::_mail.c_str(), strmailid.c_str() );

		auto validtime = KFUtility::ToValue< uint32 >( maildata[ KFField::_valid_time ] );
		if ( validtime != _invalid_int )
		{
			redisdriver->AppendCommand( "expire %s:%s %u", KFField::_mail.c_str(), strmailid.c_str(), validtime );
		}
		redisdriver->AppendCommand( "zadd %s %u %s", maillistkey.c_str(), KFMsg::FlagEnum::Init, strmailid.c_str() );
		return redisdriver->PipelineExecute();
	}

	__KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleDeleteMailReq )
	{
		__PROTO_PARSE__( KFMsg::S2SDeleteMailReq );

		RemoveMail( kfmsg.playerid(), kfmsg.mailtype(), kfmsg.mailid() );
	}

	bool KFMailShardModule::RemoveMail( uint32 playerid, uint32 mailtype, uint64 mailid )
	{
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		auto strmailid = __KF_STRING__( mailid );
		auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNCTION_LINE__ );
		if ( maillistkey == _invalid_str )
		{
			return false;
		}

		redisdriver->AppendCommand( "zrem %s %s", maillistkey.c_str(), strmailid.c_str() );

		if ( mailtype != KFMsg::MailEnum::WholeMail )
		{
			redisdriver->AppendCommand( "del %s:%s", KFField::_mail.c_str(), strmailid.c_str() );
		}

		auto ok = redisdriver->PipelineExecute();
		if ( !ok )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] player[%u] del mail[%u:%s] failed!",
				__FUNCTION_LINE__, playerid, mailtype, strmailid.c_str() );
		}

		return ok;
	}

	void KFMailShardModule::LoadWholeMailToPerson( uint32 playerid )
	{
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		// 获取玩家已经加载的最近一封GM邮件id
		std::string strmaxmailid = "";
		auto ok = redisdriver->StringExecute( strmaxmailid, "hget %s:%u %s", KFField::_mail_send_info.c_str(), playerid, KFField::_gm_email_last_id.c_str() );
		if ( !ok )
		{
			return;
		}

		if ( strmaxmailid.empty() )
		{
			strmaxmailid = "0";
		}

		// 从全局GM邮件列表中取出玩家未获取的
		VectorString querymaillist;
		redisdriver->VectorExecute( querymaillist, "zrangebyscore %s (%s +inf", KFField::_whole_mail.c_str(), strmaxmailid.c_str() );
		if ( querymaillist.empty() )
		{
			return;
		}

		auto& newmaxmailid = querymaillist.back();
		redisdriver->AppendCommand( "hset %s:%u %s %s", KFField::_mail_send_info.c_str(), playerid, KFField::_gm_email_last_id.c_str(), newmaxmailid.c_str() );

		auto maillistkey = FormatMailKeyName( playerid, KFMsg::MailEnum::WholeMail, __FUNCTION_LINE__ );
		for ( auto& strmailid : querymaillist )
		{
			redisdriver->AppendCommand( "zadd %s %u %s ", maillistkey.c_str(), KFMsg::FlagEnum::Init, strmailid.c_str() );
		}

		ok = redisdriver->PipelineExecute();
		if ( !ok )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] reload GM mail[%u] failed!",
				__FUNCTION_LINE__, playerid );
		}
	}

	bool KFMailShardModule::UpdateMailFlag( uint32 playerid, uint64 mailid, uint32 mailtype, uint32 flag )
	{
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		auto strmailid = __KF_STRING__( mailid );

		MapString maildata;
		redisdriver->MapExecute( maildata, "hgetall %s:%s", KFField::_mail.c_str(), strmailid.c_str() );
		if ( maildata.empty() )
		{
			return false;
		}

		auto oldflag = KFUtility::ToValue< uint32 >( maildata[ KFField::_flag ] );
		if ( oldflag == flag )
		{
			return false;
		}

		auto ok = false;
		switch ( flag )
		{
		case KFMsg::FlagEnum::Remove:
		case KFMsg::FlagEnum::ReceiveRemove:
		{
			ok = RemoveMail( playerid, mailtype, mailid );
		}break;
		case KFMsg::FlagEnum::Done:
		case KFMsg::FlagEnum::Received:
		{
			if ( mailtype != KFMsg::MailEnum::WholeMail )
			{
				ok = redisdriver->VoidExecute( "hset %s:%s %s %u", KFField::_mail.c_str(), strmailid.c_str(), KFField::_flag.c_str(), flag );
				if ( !ok )
				{
					KFLogger::LogLogic( KFLogger::Error, "[%s:%u] mailid[%u:%s] player[%u] flag[%u] failed!",
						__FUNCTION_LINE__, mailtype, strmailid.c_str(), playerid, flag );
				}
			}

			auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNCTION_LINE__ );
			if ( !maillistkey.empty() )
			{
				ok = redisdriver->VoidExecute( "zadd %s %u %s", maillistkey.c_str(), flag, strmailid.c_str() );
			}

		}break;
		default:
			break;
		}

		return ok;
	}

	__KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleUpdateMailFlagReq )
	{
		__PROTO_PARSE__( KFMsg::S2SUpdateMailFlagReq );
		
		auto ok = UpdateMailFlag( kfmsg.playerid(), kfmsg.mailid(), kfmsg.mailtype(), kfmsg.flag() );
		if ( !ok )
		{
			return;
		}

		KFMsg::S2SUpdateMailFlagAck ack;
		ack.set_playerid( kfmsg.playerid() );
		ack.set_flag( kfmsg.flag() );
		ack.set_mailid( kfmsg.mailid() );
		ack.set_mailtype( kfmsg.mailtype() );
		__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_UPDATE_MAIL_FLAG_ACK, &ack );
	}

	__KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleNewPlayerLoginMailReq )
	{
		__PROTO_PARSE__( KFMsg::S2SNewPlayerLoginMailReq );
		auto redisdriver = __MAIL_REDIS_DRIVER__;

		VectorString maxgmmails;
		redisdriver->VectorExecute( maxgmmails , "zrevrange %s 0 0", KFField::_whole_mail.c_str() );
		if ( maxgmmails.empty() )
		{
			return;
		}

		std::string strmaxmailid = maxgmmails[ 0 ];
		auto ok = redisdriver->VoidExecute( "hset %s:%u %s %s", KFField::_mail_send_info.c_str(), kfmsg.playerid(), KFField::_gm_email_last_id.c_str(), strmaxmailid.c_str() );
		if ( !ok )
		{
			KFLogger::LogLogic( KFLogger::Error, "[%s:%u] HandleNewPlayerLoginMailReq playerid: %u mail[%s] failed!",
				__FUNCTION_LINE__, kfmsg.playerid() , strmaxmailid.c_str() );
		}
		
	}
}