#include "KFDataProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDataProxyModule::KFDataProxyModule()
    {
    }

    KFDataProxyModule::~KFDataProxyModule()
    {

    }

    void KFDataProxyModule::InitModule()
    {

    }

    void KFDataProxyModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFDataProxyModule::OnClientLostServer );
        ////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_PROXY_REQ, &KFDataProxyModule::HandleUpdateZoneToProxyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_DATA_SHARD_REQ, &KFDataProxyModule::HandleTransmitToDataShardReq );
    }

    void KFDataProxyModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        ////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_PROXY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_DATA_SHARD_REQ );

    }

    /////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_LOST_FUNCTION__( KFDataProxyModule::OnClientLostServer )
    {
        for ( auto iter : _zone_hash._objects )
        {
            auto kfhash = iter.second;
            auto ok = kfhash->RemoveHashNode( serverid );
            if ( ok )
            {
                __LOG_ERROR__( KFLogEnum::System, "remove zone[{}] server[{}]!", iter.first, serverid );
            }
        }
    }

    uint32 KFDataProxyModule::FindZoneShardId( uint32 zoneid, uint32 clientid )
    {
        auto kfhash = _zone_hash.Find( zoneid );
        if ( kfhash == nullptr )
        {
            return  _invalid_int;
        }

        return kfhash->FindHashNode( clientid, true );
    }

    __KF_MESSAGE_FUNCTION__( KFDataProxyModule::HandleUpdateZoneToProxyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateZoneToProxyReq );

        auto shardid = __KF_HEAD_ID__( kfguid );
        for ( auto i = 0; i < kfmsg.zoneid_size(); ++i )
        {
            auto zoneid = kfmsg.zoneid( i );

            auto* kfhash = _zone_hash.Create( zoneid );
            kfhash->AddHashNode( "zone", shardid, 100 );

            __LOG_DEBUG__( KFLogEnum::System, "add zone[{}] server[{}]!", zoneid, shardid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataProxyModule::HandleTransmitToDataShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitToDataShardReq );
        auto clientid = __KF_HEAD_ID__( kfguid );

        auto shardid = FindZoneShardId( kfmsg.zoneid(), clientid );
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( KFLogEnum::System, "msgid[{}] can't find zone[{}] shardid!", kfmsg.msgid(), kfmsg.zoneid() );
        }

        auto ok = _kf_tcp_client->SendNetMessage( shardid, clientid, kfmsg.msgid(), kfmsg.msgdata().data(), kfmsg.msgdata().length() );
        if ( !ok )
        {
            __LOG_ERROR__( KFLogEnum::System, "zone[{}] shardid[{}] send msgid[{}] failed!", kfmsg.zoneid(), shardid, kfmsg.msgid() );
        }
    }
}
