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
        ////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_DATA_SHARD_REQ, &KFDataProxyModule::HandleTransmitToDataShardReq );
    }

    void KFDataProxyModule::BeforeShut()
    {
        ////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_DATA_SHARD_REQ );

    }

    /////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataProxyModule::HandleTransmitToDataShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitToDataShardReq );
        auto clientid = __KF_HEAD_ID__( kfid );

        auto shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.zoneid(), true );
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( "msgid[{}] can't find zone[{}] shardid!", kfmsg.msgid(), kfmsg.zoneid() );
        }

        auto msgdata = &kfmsg.msgdata();
        auto ok = _kf_tcp_client->SendNetMessage( shardid, clientid, kfmsg.msgid(), msgdata->data(), msgdata->length() );
        if ( !ok )
        {
            __LOG_ERROR__( "zone[{}] shardid[{}] send msgid[{}] failed!", kfmsg.zoneid(), shardid, kfmsg.msgid() );
        }
    }
}
