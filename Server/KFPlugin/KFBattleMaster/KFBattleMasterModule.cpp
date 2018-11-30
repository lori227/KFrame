#include "KFBattleMasterModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFBattleMasterModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_ALLOC_BATTLE_ID_REQ, &KFBattleMasterModule::HandleAllocBattleIdReq );
    }

    void KFBattleMasterModule::OnceRun()
    {
        _kf_battle_driver = _kf_redis->CreateExecute( __KF_STRING__( battle ) );

        auto kflist = _kf_battle_driver->QueryList( "smembers {}", __KF_STRING__( battleversionlist ) );
        for ( auto& strversion : kflist->_value )
        {
            _kf_battle_driver->Execute( "del {}:{}", __KF_STRING__( battleversion ), strversion );
        }

        _kf_battle_driver->Execute( "del {}", __KF_STRING__( battleversionlist ) );
    }

    void KFBattleMasterModule::BeforeShut()
    {
        //////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ALLOC_BATTLE_ID_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFBattleMasterModule::HandleAllocBattleIdReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAllocBattleIdReq );

        __LOG_DEBUG__( "battle[{}:{}] alloc id req!", kfmsg.ip(), kfmsg.port() );

        auto serverid = AllocBattleId( kfmsg.ip(), kfmsg.port() );

        KFMsg::S2SAllocBattleIdAck ack;
        ack.set_id( serverid );
        _kf_tcp_server->SendNetMessage( __KF_HEAD_ID__( kfid ), KFMsg::S2S_ALLOC_BATTLE_ID_ACK, &ack );

        if ( serverid != _invalid_int )
        {
            auto strserverid = KFAppID::ToString( serverid );
            __LOG_DEBUG__( "battle[{}:{}] alloc id[{}:{}] ok!", kfmsg.ip(), kfmsg.port(), serverid, strserverid );
        }
        else
        {
            __LOG_DEBUG__( "battle[{}:{}] alloc id failed!", kfmsg.ip(), kfmsg.port() );
        }
    }

    uint64 KFBattleMasterModule::AllocBattleId( const std::string& ip, uint32 port )
    {
        // 先查询是否存在
        auto kfqueryid = _kf_battle_driver->QueryString( "get {}:{}:{}", __KF_STRING__( battleid ), ip, port );
        if ( !kfqueryid->IsOk() )
        {
            return _invalid_int;
        }

        if ( !kfqueryid->_value.empty() )
        {
            return KFAppID::ToUInt64( kfqueryid->_value );
        }

        // 重新分配
        auto kfallocid = _kf_battle_driver->QueryUInt64( "incr {}", __KF_STRING__( battleidcreater ) );
        if ( !kfallocid->IsOk() || kfallocid->_value == _invalid_int )
        {
            return _invalid_int;
        }

        KFAppID kfappid( 0 );
        kfappid._union._app_data._channel_id = KFGlobal::Instance()->_app_id._union._app_data._channel_id;
        kfappid._union._app_data._zone_id = 0;
        kfappid._union._app_data._server_type = KFServerEnum::BattleServer;
        kfappid._union._app_data._worker_id = kfallocid->_value;

        // 保存数据库
        auto strserverid = kfappid.ToString();
        _kf_battle_driver->Execute( "set {}:{}:{} {}", __KF_STRING__( battleid ), ip, port, strserverid );

        return kfappid._union._id;
    }
}