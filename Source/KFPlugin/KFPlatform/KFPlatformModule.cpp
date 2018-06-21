#include "KFPlatformModule.h"
#include "KFJson.h"
#include "KFPlatformConfig.h"
#include "KFZoneManage.h"
#include "KFConvert/KFConvert.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->CreateExecute( KFField::_account )

	KFPlatformModule::KFPlatformModule()
	{
	}

	KFPlatformModule::~KFPlatformModule()
	{
	}

	void KFPlatformModule::InitModule()
	{
		//////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_platform_config, _kf_plugin->_plugin_name, _kf_plugin->_config, true );
	}

	void KFPlatformModule::BeforeRun()
	{
		__REGISTER_RUN_FUNCTION__( &KFPlatformModule::Run );
		_kf_zone_manage->BeforeRun();

		__REGISTER_HTTP_FUNCTION__( KFField::_login, false, &KFPlatformModule::HandleLoginToken );
		__REGISTER_HTTP_FUNCTION__( KFField::_token, false, &KFPlatformModule::HandleVerifyToken );
		__REGISTER_HTTP_FUNCTION__( KFField::_update_zone, false, &KFPlatformModule::HandleUpdateZone );
		__REGISTER_HTTP_FUNCTION__( KFField::_lost_zone, false, &KFPlatformModule::HandleLostZone );
		__REGISTER_HTTP_FUNCTION__( KFField::_status, false, &KFPlatformModule::HandleUpdateZoneStatus );
		__REGISTER_HTTP_FUNCTION__( KFField::_zone_list, false, &KFPlatformModule::HandleQueryZoneList );
		__REGISTER_HTTP_FUNCTION__( KFField::_zone_status, false, &KFPlatformModule::HandleQueryZoneStatus );
		__REGISTER_HTTP_FUNCTION__( KFField::_activation, false, &KFPlatformModule::HandleActivationAccount );
		__REGISTER_HTTP_FUNCTION__( KFField::_zone_http, false, &KFPlatformModule::HandleZoneHttp );
		__REGISTER_HTTP_FUNCTION__( KFField::_online_zone, false, &KFPlatformModule::HandleUpdateOnline );
	}

	void KFPlatformModule::BeforeShut()
	{
		__UNREGISTER_RUN_FUNCTION__();
		_kf_zone_manage->BeforeShut();
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );

		__UNREGISTER_HTTP_FUNCTION__( KFField::_login );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_token );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_update_zone );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_lost_zone );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_status );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_zone_list );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_zone_status );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_activation );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_zone_http );
		__UNREGISTER_HTTP_FUNCTION__( KFField::_online_zone );
	}

	void KFPlatformModule::Run()
	{
		_kf_zone_manage->Run();
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	std::string KFPlatformModule::HandleLoginToken( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		auto account = request.GetString( KFField::_account );
		auto channel = request.GetUInt32( KFField::_channel );
		if ( account.empty() )
		{
			return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
		}
		
		// 查询创建账号
		auto accountdata = QueryCreateAccount( account, channel );
		auto accountid = KFUtility::ToValue<uint32>( accountdata[ KFField::_account_id ] );
		if ( accountid == 0 )
		{
			return _kf_http_server->SendResponseCode( KFMsg::PlatformBusy );
		}

		// 判断是否需要激活
		if ( _kf_platform_config->_is_open_activation )
		{
			auto activation = accountdata[ KFField::_activation ];
			if ( activation.empty() )
			{
				KFJson response;
				response.SetValue( KFField::_account_id, accountid );
				response.SetValue< uint32 >( KFField::_ret_code, KFMsg::ActivationAccount );
				return _kf_http_server->SendResponse( response );
			}
		}

		// 保存额外数据
		UpdateExtendData( accountid, channel, request );

		// 保存token
		return SaveLoginToken( accountid, accountdata );
	}

	void KFPlatformModule::UpdateExtendData( uint32 accountid, uint32 channel, KFJson& kfjson )
	{
		MapString values;
		values[ KFField::_account_id ] = __KF_STRING__( accountid );
		values[ KFField::_channel ] = __KF_STRING__( channel );
		values[ KFField::_name ] = kfjson.GetString( KFField::_name );
		values[ KFField::_sex ] = kfjson.GetString( KFField::_sex );
		values[ KFField::_icon ] = kfjson.GetString( KFField::_icon );

		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
		redisdriver->VoidExecute( values, "hmset %s:%u", KFField::_extend.c_str(), accountid );
	}

	std::string KFPlatformModule::SaveLoginToken( uint32 accountid, MapString& accountdata )
	{
		auto accountflag = KFUtility::ToValue<uint32>( accountdata[ KFField::_account_flag ] );

		// 创建token
		auto strtime = __KF_STRING__( KFGlobal::Instance()->_game_time );
		auto md5temp = KFUtility::Format( "%u%u%s", accountid, accountflag, strtime.c_str() );
		auto token = KFCrypto::Md5Encode( md5temp );

		// 保存登录token, 设置有效时间300秒
		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
		auto rediskey = KFUtility::Format( "%s:%s", KFField::_login.c_str(), token.c_str() );
		redisdriver->AppendCommand( "hset %s %s %u", rediskey, KFField::_account_id.c_str(), accountid );
		redisdriver->AppendCommand( "expire %s %u", rediskey, _kf_platform_config->_token_expire_time );
		redisdriver->PipelineExecute();
		
		// 返回内容
		KFJson response;
		response.SetValue( KFField::_token, token );
		response.SetValue( KFField::_account_id, accountid );
		response.SetValue( KFField::_account_flag, accountflag );
		response.SetValue( KFField::_server_list_type, _kf_platform_config->_server_list_type );

		switch ( _kf_platform_config->_server_list_type )
		{
		case __SELECT_SERVER_DATA__:
		{
			auto zoneid = KFUtility::ToValue<uint32>( accountdata[ KFField::_zone_id ] );
			if ( zoneid == _invalid_int )
			{
				// 选择一个最小人数的分区
				VectorString zonevalues;
				redisdriver->VectorExecute( zonevalues, "zrevrange %s 0 0", KFField::_zone_balance.c_str() );
				if ( zonevalues.empty() )
				{
					return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
				}

				zoneid = KFUtility::ToValue< uint32 >( zonevalues.front() );
				if ( zoneid == _invalid_int )
				{
					return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
				}

				redisdriver->AppendCommand( "hset %s:%u %s %u",
					KFField::_account_id.c_str(), accountid, KFField::_zone_id.c_str(), zoneid );
				redisdriver->AppendCommand( "zincrby %s 1 %u", KFField::_zone_balance.c_str(), 1 );
				redisdriver->PipelineExecute();
			}

			std::string ip = "";
			uint32 port = 0;

			_kf_zone_manage->RandZoneJson( zoneid, ip, port );
			if ( ip.empty() || port == 0 )
			{
				return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
			}

			response.SetValue( KFField::_ip, ip );
			response.SetValue( KFField::_port, port );
		}break;
		default:
		{
			KFJson kfserverlistjson;
			_kf_zone_manage->GetZoneJson( kfserverlistjson );
			response.SetValue< Json::Value& >( KFField::_server_list, kfserverlistjson );
		}break;
		}

		// 判断是否已经在线
		auto online = accountdata[ KFField::_online ];
		if ( online == "1" )
		{
			auto zoneid = KFUtility::ToValue< uint32 >( accountdata[ KFField::_zone_id ] );
			auto url = _kf_zone_manage->GetZoneUrl( zoneid );
			if ( !url.empty() )
			{
				KFJson kfjson;
				kfjson.SetValue( KFField::_player_id, accountdata[ KFField::_player_id ] );
				
				auto kickurl = url + KFField::_kick_online;
				_kf_http_client->StartMTHttpClient( kickurl, kfjson, false );
			}
		}

		return _kf_http_server->SendResponse( response );
	}

	std::string KFPlatformModule::HandleActivationAccount( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		auto accountid = request.GetUInt32( KFField::_account_id );
		if ( accountid == _invalid_int )
		{
			return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
		}

		// 查询创建账号
		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

		MapString accountdata;
		auto ok = redisdriver->MapExecute( accountdata, "hgetall %s:%u", KFField::_account_id.c_str(), accountid );
		if ( !ok || accountdata.empty() )
		{
			return _kf_http_server->SendResponseCode( KFMsg::PlatformBusy );
		}

		auto activation = accountdata[ KFField::_activation ];
		if ( activation.empty() )
		{
			// 保存激活码
			auto code = request.GetString( KFField::_code );

			// 判断激活码是否存在

			uint64 isexist = 0;
			if ( !redisdriver->UInt64Execute( isexist, "sismember %s %s", KFField::_activation_code.c_str(), code.c_str() ) )
			{
				return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
			}

			if ( isexist == 0 )
			{
				return _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
			}

			redisdriver->AppendCommand( "srem %s %s", KFField::_activation_code.c_str(), code.c_str() );
			redisdriver->AppendCommand( "hset %s:%u %s %s",
				KFField::_account_id.c_str(), accountid, KFField::_activation.c_str(), code.c_str() );
			redisdriver->PipelineExecute();
		}

		return SaveLoginToken( accountid, accountdata );
	}
	
	std::string KFPlatformModule::HandleQueryZoneList( const std::string& ip, const std::string& data )
	{
		KFJson kfserverlistjson;
		_kf_zone_manage->GetZoneJson( kfserverlistjson );

		KFJson response;
		response.SetValue< Json::Value& >( KFField::_server_list, kfserverlistjson );
		return _kf_http_server->SendResponse( response );
	}

	MapString KFPlatformModule::QueryCreateAccount( const std::string& account, uint32 channel )
	{
		MapString accountdata;
		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

		// 先查询redis	
		std::string queryvalue;
		if ( !redisdriver->StringExecute( queryvalue, "hget %s:%s:%u %s", KFField::_account.c_str(), account.c_str(), channel, KFField::_account_id.c_str() ) )
		{
			return accountdata;
		}

		if ( queryvalue != FUNCTION_EMPTY_STRING )
		{
			uint32 accountid = KFUtility::ToValue<uint32>( queryvalue );
			auto ok = redisdriver->MapExecute( accountdata, "hgetall %s:%u", KFField::_account_id.c_str(), accountid );
			if ( !ok || !accountdata.empty() )
			{
				return accountdata;
			}
		}
		
		// 创建账号id
		uint64 newid = 0;
		redisdriver->UInt64Execute( newid, "incr %s", KFField::_account_make.c_str() );
		if ( newid == 0 )
		{
			return accountdata;
		}

		auto accountid = static_cast< uint32 >( newid ) + 500000;

		accountdata[ KFField::_account_id ] = KFUtility::ToString( accountid );
		accountdata[ KFField::_account_flag ] = "0";
		accountdata[ KFField::_account ] = account;
		accountdata[ KFField::_channel ] = KFUtility::ToString( channel );
		
		redisdriver->AppendCommand( "hset %s:%s:%u %s %u", KFField::_account.c_str(), account.c_str(), channel, KFField::_account_id.c_str(), accountid );
		redisdriver->AppendCommand( "sadd %s %u", KFField::_account_list.c_str(), accountid );
		redisdriver->AppendCommand( accountdata, "hmset %s:%u", KFField::_account_id.c_str(), accountid );
		redisdriver->PipelineExecute();
		return accountdata;
	}
	
	std::string KFPlatformModule::HandleVerifyToken( const std::string& ip, const std::string& data )
	{
		KFJson request( data );
		auto token = request.GetString( KFField::_token );
		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

		// 判断token是否正确
		auto loginkey = KFUtility::Format( "%s:%s", KFField::_login.c_str(), token.c_str() );

		std::string queryvalue = "";
		if ( !redisdriver->StringExecute( queryvalue, "hget %s %s", loginkey, KFField::_account_id.c_str() ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
		}

		// 删除token
		redisdriver->VoidExecute( "del %s", loginkey );

		// 获得accountid
		auto accountid = KFUtility::ToValue<uint32>( queryvalue );
		if ( accountid == 0 )
		{
			return _kf_http_server->SendResponseCode( KFMsg::LoginTokenError );
		}
		
		MapString accountdata;
		if ( !redisdriver->MapExecute( accountdata, "hgetall %s:%u", KFField::_account_id.c_str(), accountid ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
		}

		// 设置账号
		KFJson response;
		response.SetValue( KFField::_token, token );
		response.SetValue( KFField::_account_id, accountid );
		response.SetValue( KFField::_channel, accountdata[ KFField::_channel ] );
		return _kf_http_server->SendResponse( response );
	}

	std::string KFPlatformModule::HandleUpdateZone( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		// 计算签名
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		// 更新列表
		_kf_zone_manage->AddZoneData( request );
		return _kf_http_server->SendResponseCode( KFMsg::Success );
	}

	std::string KFPlatformModule::HandleZoneHttp( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		// 计算签名
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		auto zoneid = request.GetUInt32( KFField::_zone_id );
		auto url = request.GetString( KFField::_url );

		// 更新zone http
		_kf_zone_manage->UpdateZoneHttp( zoneid, url );
		return _kf_http_server->SendResponseCode( KFMsg::Success );
	}
	
	std::string KFPlatformModule::HandleLostZone( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		// 计算签名
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		auto zoneid = request.GetUInt32( KFField::_id );
		auto appid = request.GetUInt32( KFField::_app_id );
		_kf_zone_manage->RemoveZoneAddress( zoneid, appid );

		return _kf_http_server->SendResponseCode( KFMsg::Success );
	}

	std::string KFPlatformModule::HandleUpdateZoneStatus( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		auto zoneid = request.GetUInt32( KFField::_id );
		auto zonestatus = request.GetUInt32( KFField::_status );
		auto describe = request.GetString( KFField::_describe );
	
		// 计算签名
		if ( !_kf_http_server->VerifySignature( request ) )
		{
			return _kf_http_server->SendResponseCode( KFMsg::SignError );
		}

		// 更新列表
		_kf_zone_manage->UpdateZoneStatus( zoneid, zonestatus, describe );
		return _kf_http_server->SendResponseCode( KFMsg::Success );
	}
	
	std::string KFPlatformModule::HandleQueryZoneStatus( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		auto zoneid = request.GetUInt32( KFField::_id );
		
		uint32 status = KFZoneStatusEnum::Shutoff;
		std::string describe = "";
		_kf_zone_manage->QueryZoneStatus( zoneid, status, describe );

		KFJson response;
		response.SetValue( KFField::_id, zoneid );
		response.SetValue( KFField::_status, status );
		response.SetValue( KFField::_describe, describe );
		return _kf_http_server->SendResponse( response );
	}

	std::string KFPlatformModule::HandleUpdateOnline( const std::string& ip, const std::string& data )
	{
		KFJson request( data );

		auto accountid = request.GetString( KFField::_account_id );
		auto playerid = request.GetString( KFField::_player_id );
		auto online = request.GetString( KFField::_online );
		auto zoneid = request.GetString( KFField::_zone_id );

		MapString values;
		values[ KFField::_player_id ] = playerid;
		values[ KFField::_online ] = online;
		values[ KFField::_zone_id ] = zoneid;

		auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
		redisdriver->VoidExecute( values, "hmset %s:%s", KFField::_account_id.c_str(), accountid.c_str() );
		return _kf_http_server->SendResponseCode( KFMsg::Success );
	}
}
