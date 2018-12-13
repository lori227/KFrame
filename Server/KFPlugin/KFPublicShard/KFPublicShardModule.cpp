#include "KFPublicShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendToClient( kfid, msgid, message )

    void KFPublicShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ, &KFPublicShardModule::HandleCreateRoleReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ, &KFPublicShardModule::HandleUpdatePublicDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ, &KFPublicShardModule::HandleQueryBasicDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ, &KFPublicShardModule::HandleSetPlayerNameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ, &KFPublicShardModule::HandleUpdateGuestListReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ, &KFPublicShardModule::HandleQueryGuestReq );
    }

    void KFPublicShardModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_ROLE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_PUBLIC_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_PLAYER_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GUEST_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUEST_REQ );
    }


    void KFPublicShardModule::OnceRun()
    {
        _name_redis_driver = _kf_redis->Create( __KF_STRING__( name ) );
        _public_redis_driver = _kf_redis->Create( __KF_STRING__( public ) );
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

        _public_redis_driver->Update( values, "hmset {}:{}", __KF_STRING__( public ), kfmsg.playerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryBasicDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryBasicReq );

        auto queryid = _name_redis_driver->QueryString( "get {}:{}", __KF_STRING__( name ), kfmsg.name() );

        KFMsg::S2SQueryBasicAck ack;
        ack.set_playerid( kfmsg.playerid() );

        auto pbobject = ack.mutable_pbobject();
        pbobject->set_key( KFUtility::ToValue< uint64>( queryid->_value ) );

        // 查询所有数据
        if ( !queryid->_value.empty() )
        {
            auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), queryid->_value );
            for ( auto& iter : querydata->_value )
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
        ack.set_result( result );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_newname( kfmsg.newname() );
        ack.set_modleid( kfmsg.modleid() );
        ack.set_sex( kfmsg.sex() );
        ack.set_inviterid( kfmsg.inviterid() );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_CREATE_ROLE_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleSetPlayerNameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSetPlayerNameReq );

        // 先查询名字
        uint32 result = ProcessSetPlayerName( kfmsg.playerid(), kfmsg.oldname(), kfmsg.newname() );

        KFMsg::S2SSetPlayerNameAck ack;
        ack.set_result( result );
        ack.set_playerid( kfmsg.playerid() );
        ack.set_oldname( kfmsg.oldname() );
        ack.set_newname( kfmsg.newname() );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_SET_PLAYER_NAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdateGuestListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateGuestListReq );

        // 保存访客信息
        _public_redis_driver->Execute( "zadd {}:{} {} {}", __KF_STRING__( guest ), kfmsg.playerid(), kfmsg.guesttime(), kfmsg.guestid() );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryGuestReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryGuestReq );

        // 删除过期访客信息
        auto querytime = kfmsg.querytime();
        uint64 validtime = KFTimeEnum::OneDaySecond * KFTimeEnum::OneMonthDay;
        if ( querytime > validtime )
        {
            _public_redis_driver->Execute( "zremrangebyscore {}:{} {} {}", __KF_STRING__( guest ), kfmsg.queryid(), _invalid_int, querytime - validtime );
        }

        // 返回访客信息
        KFMsg::S2SQueryGuestAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_queryid( kfmsg.queryid() );

        // 获取访客数量
        auto querycount = _public_redis_driver->QueryUInt64( "zcard {}:{}", __KF_STRING__( guest ), kfmsg.queryid() );
        ack.set_guestcount( querycount->_value );

        // 获取最近四位访客的信息
        if ( querycount->_value != _invalid_int )
        {
            auto guestlist = _public_redis_driver->QueryList( "zrevrange {}:{} 0 3", __KF_STRING__( guest ), kfmsg.queryid() );
            for ( auto& strid : guestlist->_value )
            {
                auto guestid = KFUtility::ToValue< uint64 >( strid );

                auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), guestid );

                auto pbguestdata = ack.add_guestdata();
                pbguestdata->set_guestid( guestid );
                pbguestdata->set_guesticon( querydata->_value[ __KF_STRING__( icon ) ] );
                pbguestdata->set_guesticonbox( querydata->_value[ __KF_STRING__( iconbox ) ] );
            }
        }

        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_GUEST_ACK, &ack );
    }

    uint32 KFPublicShardModule::ProcessSetPlayerName( uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto queryplayerid = _name_redis_driver->QueryString( "get {}:{}", __KF_STRING__( name ), newname );
        if ( !queryplayerid->IsOk() )
        {
            return KFMsg::PublicDatabaseError;
        }

        // 如果已经设置, 并且不是自己, 返回错误
        auto queryid = KFUtility::ToValue< uint64 >( queryplayerid->_value );
        if ( queryid != _invalid_int && queryid != playerid )
        {
            return KFMsg::NameAlreadyExist;
        }

        // 保存名字
        auto kfresult = _name_redis_driver->Execute( "set {}:{} {}", __KF_STRING__( name ), newname, playerid );
        if ( !kfresult->IsOk() )
        {
            return KFMsg::PublicDatabaseError;
        }

        // 删除旧的名字关联
        if ( !oldname.empty() )
        {
            _name_redis_driver->Execute( "del {}:{}", __KF_STRING__( name ), oldname );
        }

        return KFMsg::Success;
    }

}