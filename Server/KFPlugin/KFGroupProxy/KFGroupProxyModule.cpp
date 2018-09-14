#include "KFGroupProxyModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFGroupProxyModule::KFGroupProxyModule()
    {
    }

    KFGroupProxyModule::~KFGroupProxyModule()
    {
    }

    void KFGroupProxyModule::InitModule()
    {
        ///////////////////////////////////////////////////////////////////////////////

    }

    void KFGroupProxyModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_MATCH_GROUP_REQ, &KFGroupProxyModule::HandleCreateMatchGroupReq );
    }

    void KFGroupProxyModule::BeforeShut()
    {
        /////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_MATCH_GROUP_REQ );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFGroupProxyModule::HandleCreateMatchGroupReq )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateMatchGroupReq );
        __LOG_DEBUG__( KFLogEnum::Logic, "create group[{}] req!", kfmsg.groupid() );

        auto shardid = _kf_cluster_proxy->SelectClusterShard( kfmsg.groupid() );
        if ( shardid == _invalid_int )
        {
            return __LOG_ERROR__( KFLogEnum::Logic, "group[{}] can't find shard!", kfmsg.groupid() );
        }

        // 发送给分片服务器
        KFMsg::S2SCreateMatchGroupToShardReq req;
        req.set_groupid( kfmsg.groupid() );
        req.set_maxcount( kfmsg.maxcount() );
        req.set_matchid( kfmsg.matchid() );
        req.set_playerid( kfmsg.playerid() );
        req.mutable_pbmember()->CopyFrom( kfmsg.pbmember() );
        auto ok = _kf_cluster_proxy->SendMessageToShard( shardid, KFMsg::S2S_CREATE_MATCH_GROUP_TO_SHARD_REQ, &req );
        if ( ok )
        {
            // 先添加到列表
            _kf_cluster_proxy->AddDynamicShard( kfmsg.groupid(), shardid );

            __LOG_DEBUG__( KFLogEnum::Logic, "create group[{}] ok!", kfmsg.groupid() );
        }
        else
        {
            __LOG_ERROR__( KFLogEnum::Logic, "create group[{}] failed!", kfmsg.groupid() );
        }
    }

}