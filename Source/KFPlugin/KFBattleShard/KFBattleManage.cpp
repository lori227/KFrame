#include "KFBattleManage.h"

namespace KFrame
{
	KFBattleServer::KFBattleServer()
	{
		Reset();
	}

	KFBattleServer::~KFBattleServer()
	{

	}

	void KFBattleServer::Reset()
	{
		_server_id = _invalid_int;
		_proxy_id = _invalid_int;
		_ip.clear();
		_port = _invalid_int;
	}

	bool KFBattleServer::IsValid() const
	{
		if ( _server_id == _invalid_int ||
			_proxy_id == _invalid_int ||
			_ip.empty() ||
			_port == _invalid_int )
		{
			return false;
		}

		return true;
	}

	void KFBattleManage::Initialize()
	{
		_redis_driver = _kf_redis->CreateExecute( KFField::_battle );
	}

	/////////////////////////////////////////////////////////////////////////
	void KFBattleManage::RegisterBattleServer( uint32 serverid, uint32 proxyid, const std::string& ip, uint32 port )
	{
		// 为了避免单点问题, 保存到redis缓存中
		// 先删除列表在添加到列表中
		
		MapString values;
		values[ KFField::_ip ] = ip;
		values[ KFField::_port ] = __KF_STRING__( port );
		values[ KFField::_proxy_id ] = __KF_STRING__( proxyid );
		values[ KFField::_server_id ] = __KF_STRING__( serverid );
		_redis_driver->AppendCommand( values, "hmset %s:%u", KFField::_battle.c_str(), serverid );
		_redis_driver->AppendCommand( "sadd %s:%s %u", KFField::_ip.c_str(), ip.c_str(), serverid );
		_redis_driver->PipelineExecute();

		// 设置数量
		UpdateBattleCount( ip );
	}

	void KFBattleManage::UnRegisterBattleServer( uint32 serverid )
	{
		std::string queryip = "";
		_redis_driver->StringExecute( queryip, "hget %s:%u %s", KFField::_battle.c_str(), serverid, KFField::_ip.c_str() );
		if ( queryip.empty() )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] redis can't find battle[%u] ip!",
				__FUNCTION__, serverid );
		}

		_redis_driver->AppendCommand( "del %s:%u", KFField::_battle.c_str(), serverid );
		_redis_driver->AppendCommand( "srem %s:%s %u", KFField::_ip.c_str(), queryip.c_str(), serverid );
		_redis_driver->PipelineExecute();
		
		KFLogger::LogLogic( KFLogger::Debug, "[%s] unregister battle server[%u|%s]!",
			__FUNCTION__, serverid, queryip.c_str() );

		// 设置数量
		UpdateBattleCount( queryip );
	}

	void KFBattleManage::AllocBattleServer( KFBattleServer* battleserver )
	{
		// 找到负载最小的一个
		VectorString ipvalues;
		_redis_driver->VectorExecute( ipvalues, "zrevrange %s 0 0", KFField::_battle_list.c_str() );
		if ( ipvalues.empty() )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] no battle server!", 
				__FUNCTION__ );
		}

		auto& queryip = ipvalues.front();

		// 弹出一个可用的服务器
		std::string queryserverid = "";
		_redis_driver->StringExecute( queryserverid, "spop %s:%s", KFField::_ip.c_str(), queryip.c_str() );
		if ( queryserverid.empty() )
		{
			_redis_driver->VoidExecute( "zrem %s %s", KFField::_battle_list.c_str(), queryip.c_str() );
			return KFLogger::LogLogic( KFLogger::Error, "[%s] [%s] battle list empty!",
				__FUNCTION__, queryip.c_str() );
		}

		// 数量减1
		_redis_driver->VoidExecute( "zincrby %s -1 %s", KFField::_battle_list.c_str(), queryip.c_str() );

		// 查询所有信息
		MapString values;
		_redis_driver->MapExecute( values, "hgetall %s:%s", KFField::_battle.c_str(), queryserverid.c_str() );
		if ( values.empty() )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] [%s] battle data empty!",
				__FUNCTION__, queryserverid.c_str() );
		}

		auto ip = values[ KFField::_ip ];
		auto port = KFUtility::ToValue< uint32 >( values[ KFField::_port ] );
		auto serverid = KFUtility::ToValue< uint32 >( values[ KFField::_server_id ] );
		auto proxyid = KFUtility::ToValue< uint32 >( values[ KFField::_proxy_id ] );
		if ( ip.empty() || port == _invalid_int || proxyid == _invalid_int || serverid == _invalid_int )
		{
			return KFLogger::LogLogic( KFLogger::Error, "[%s] battle data [%s:%u:%u] error!",
				__FUNCTION__, ip.c_str(), port, proxyid );
		}

		// 保存数据./
		battleserver->_ip = ip;
		battleserver->_port = port;
		battleserver->_proxy_id = proxyid;
		battleserver->_server_id = serverid;
	}
		
	void KFBattleManage::RemoveBattleServer( uint32 serverid, const std::string& ip )
	{
		_redis_driver->VoidExecute( "srem %s:%s %u", KFField::_ip.c_str(), ip.c_str(), serverid );

		// 设置数量
		UpdateBattleCount( ip );

		KFLogger::LogLogic( KFLogger::Debug, "[%s] remove battle server[%u|%s]!",
			__FUNCTION__, serverid, ip.c_str() );
	}

	void KFBattleManage::FreeBattleServer( KFBattleServer* battleserver )
	{
		if ( !battleserver->IsValid() )
		{
			return;
		}
		
		FreeBattleServer( battleserver->_server_id, battleserver->_ip );
		battleserver->Reset();
	}

	void KFBattleManage::FreeBattleServer( uint32 serverid, const std::string& ip )
	{
		KFLogger::LogLogic( KFLogger::Debug, "[%s] free battle server[%u|%s]!",
			__FUNCTION__, serverid, ip.c_str() );

		_redis_driver->VoidExecute( "sadd %s:%s %u", KFField::_ip.c_str(), ip.c_str(), serverid );

		// 更新数量
		UpdateBattleCount( ip );
	}

	void KFBattleManage::UpdateBattleCount( const std::string& ip )
	{
		uint64 battlecount = 0;
		if ( _redis_driver->UInt64Execute( battlecount, "scard %s:%s", KFField::_ip.c_str(), ip.c_str() ) )
		{
			_redis_driver->VoidExecute( "zadd %s %u %s", KFField::_battle_list.c_str(), battlecount, ip.c_str() );
		
			KFLogger::LogLogic( KFLogger::Debug, "[%s] ip[%s] server count[%u]!",
				__FUNCTION__, ip.c_str(), static_cast< uint32 >( battlecount ) );
		}
	}
}

