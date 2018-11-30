#include "KFBattleManage.h"

namespace KFrame
{
    KFBattleServer::KFBattleServer()
    {
        Reset();
    }

    void KFBattleServer::Reset()
    {
        _ip.clear();
        _port = _invalid_int;
        _proxy_id = _invalid_int;
        _server_id = _invalid_int;
        _version.clear();
    }

    bool KFBattleServer::IsValid() const
    {
        if ( _server_id == _invalid_int ||
                _proxy_id == _invalid_int ||
                _ip.empty() ||
                _port == _invalid_int ||
                _version.empty() )
        {
            return false;
        }

        return true;
    }
    /////////////////////////////////////////////////////////////////////////
    KFBattleManage::KFBattleManage()
    {
        _battle_redis_driver = nullptr;
    }

    void KFBattleManage::Initialize()
    {
        _battle_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( battle ) );
    }

    void KFBattleManage::RegisterBattleServer( uint64 serverid, uint64 proxyid, const std::string& ip, uint32 port, const std::string& version )
    {
        // 为了避免单点问题, 保存到redis缓存中
        // 先删除列表在添加到列表中
        MapString values;
        values[ __KF_STRING__( ip ) ] = ip;
        values[ __KF_STRING__( port ) ] = __TO_STRING__( port );
        values[ __KF_STRING__( proxyid ) ] = __TO_STRING__( proxyid );
        values[ __KF_STRING__( serverid ) ] = __TO_STRING__( serverid );
        values[ __KF_STRING__( version ) ] = version;

        _battle_redis_driver->Append( values, "hmset {}:{}", __KF_STRING__( battle ), serverid );
        _battle_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( battlelist ), version, serverid );
        _battle_redis_driver->Append( "sadd {}:{} {}", __KF_STRING__( battleversion ), version, serverid );
        _battle_redis_driver->Append( "sadd {} {}", __KF_STRING__( battleversionlist ), version );

        _battle_redis_driver->Pipeline();
    }

    void KFBattleManage::UnRegisterBattleServer( uint64 serverid )
    {
        auto kfresult = _battle_redis_driver->QueryString( "hget {}:{} {}", __KF_STRING__( battle ), serverid, __KF_STRING__( version ) );
        if ( kfresult->_value.empty() )
        {
            return;
        }

        _battle_redis_driver->Append( "del {}:{}", __KF_STRING__( battle ), serverid );
        _battle_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( battlelist ), kfresult->_value, serverid );
        _battle_redis_driver->Append( "srem {}:{} {}", __KF_STRING__( battleversion ), kfresult->_value, serverid );
        _battle_redis_driver->Pipeline();

        auto kfcount = _battle_redis_driver->QueryUInt64( "scard {}:{}", __KF_STRING__( battleversion ), kfresult->_value );
        if ( kfcount->_value == 0 )
        {
            _battle_redis_driver->Execute( "srem {} {}", __KF_STRING__( battleversionlist ), kfresult->_value );
        }
        __LOG_DEBUG__( "unregister battle server[{}] version[{}]!", serverid, kfresult->_value );
    }

    void KFBattleManage::AllocBattleServer( uint64 battleserverid, const std::string& version, KFBattleServer* battleserver )
    {
        // 找到负载最小的一个
        auto queryserverid = battleserverid;
        if ( queryserverid == _invalid_int )
        {
            // 弹出一个可用的服务器
            auto kfquery = _battle_redis_driver->QueryUInt32( "spop {}:{}", __KF_STRING__( battlelist ), version );
            if ( kfquery->_value == _invalid_int )
            {
                return __LOG_ERROR__( "version[{}] battle list empty!", version );
            }

            queryserverid = kfquery->_value;
        }

        // 查询所有信息
        auto mapresult = _battle_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( battle ), queryserverid );
        if ( mapresult->_value.empty() )
        {
            return __LOG_ERROR__( "[{}] battle data empty!", queryserverid );
        }

        auto ip = mapresult->_value[ __KF_STRING__( ip ) ];
        auto port = KFUtility::ToValue< uint32 >( mapresult->_value[ __KF_STRING__( port ) ] );
        auto serverid = KFUtility::ToValue< uint32 >( mapresult->_value[ __KF_STRING__( serverid ) ] );
        auto proxyid = KFUtility::ToValue< uint32 >( mapresult->_value[ __KF_STRING__( proxyid ) ] );
        if ( ip.empty() || port == _invalid_int || proxyid == _invalid_int || serverid == _invalid_int )
        {
            return __LOG_ERROR__( "battle data [{}:{}:{}] error!", ip, port, proxyid );
        }

        // 保存数据 /
        battleserver->_ip = ip;
        battleserver->_port = port;
        battleserver->_proxy_id = proxyid;
        battleserver->_server_id = serverid;
        battleserver->_version = version;

        if ( battleserverid != _invalid_int )
        {
            // 如果是指定战场, 删除列表
            _battle_redis_driver->Execute( "srem {}:{} {}", __KF_STRING__( ip ), ip, battleserverid );
        }
    }

    void KFBattleManage::RemoveBattleServer( uint64 serverid, const std::string& version )
    {
        _battle_redis_driver->Execute( "srem {}:{} {}", __KF_STRING__( battlelist ), version, serverid );

        __LOG_DEBUG__( "remove battle server[{}] version[{}]!", serverid, version );
    }

    void KFBattleManage::FreeBattleServer( uint64 serverid, const std::string& version )
    {
        __LOG_DEBUG__( "free battle server[{}] version[{}]!", serverid, version );

        _battle_redis_driver->Execute( "sadd {}:{} {}", __KF_STRING__( battlelist ), version, serverid );
    }

}

