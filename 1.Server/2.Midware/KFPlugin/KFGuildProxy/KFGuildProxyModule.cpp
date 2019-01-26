#include "KFGuildProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFGuildProxyModule::KFGuildProxyModule()
    {
    }

    KFGuildProxyModule::~KFGuildProxyModule()
    {

    }

    void KFGuildProxyModule::InitModule()
    {

    }

    void KFGuildProxyModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_REQ, &KFGuildProxyModule::HandleCreateGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LIST_REQ, &KFGuildProxyModule::HandleQueryGuildListReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ, &KFGuildProxyModule::HandleSearchGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ, &KFGuildProxyModule::HandleQueryGuildidReq );
    }

    void KFGuildProxyModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildProxyModule ::HandleCreateGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateGuidReq );
        auto guildid = KFGlobal::Instance()->Make64Guid();

        auto shardid = _kf_cluster_proxy->FindMinObjectShard();
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "guildid[{}] can't find shard!", guildid );
        }

        KFMsg::S2SCreateGuildToShardReq req;
        req.set_guildid( guildid );
        req.set_serverid( kfmsg.serverid() );
        req.set_playerid( kfmsg.playerid() );
        req.set_name( kfmsg.name() );
        req.set_guildname( kfmsg.guildname() );
        req.set_medal( kfmsg.medal() );
        req.set_manifesto( kfmsg.manifesto() );

        auto ok = _kf_cluster_proxy->SendToShard( shardid, KFMsg::S2S_CREATE_GUILD_TO_SHARD_REQ, &req );
        if ( ok )
        {
            // 先不添加到列表，可能失败
            // _kf_cluster_proxy->AddObjectShard( guildid, shardid );

            __LOG_DEBUG__( "create guild[{}] ok!", guildid );
        }
        else
        {
            __LOG_ERROR__( "create guild[{}] failed!", guildid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildProxyModule::HandleQueryGuildListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryGuildListReq );
        auto shardid = _kf_cluster_proxy->FindMinObjectShard();
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "query guildlist can't find shard!" );
        }
        auto ok = _kf_cluster_proxy->SendToShard( shardid, KFMsg::S2S_QUERY_GUILD_LIST_REQ, data, length );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildProxyModule::HandleSearchGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSearchGuildByNameReq );
        auto shardid = _kf_cluster_proxy->FindMinObjectShard();
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "search guild can't find shard!" );
        }
        auto ok = _kf_cluster_proxy->SendToShard( shardid, KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ, data, length );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildProxyModule::HandleQueryGuildidReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryGuildidReq );
        auto shardid = _kf_cluster_proxy->FindMinObjectShard();
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "query guild id can't find shard!" );
        }
        auto ok = _kf_cluster_proxy->SendToShard( shardid, KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ, data, length );
    }
}
