#ifndef __KF_RANK_CLIENT_INTERFACE_H__
#define __KF_RANK_CLIENT_INTERFACE_H__

#include "KFKernel/KFEntity.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFRankClientInterface : public KFModule
    {
    public:
        // 发送消息到Rank
        virtual bool SendMessageToRank( uint64 zoneid, uint32 msgid, ::google::protobuf::Message* message ) = 0;
        virtual void JoinRankList( uint64 matchid, uint32 playerid, KFEntity* player, const std::string& scoretype ) = 0;
        virtual bool SendMessageToRankShard( uint32 msgid, ::google::protobuf::Message* message ) = 0;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_rank, KFRankClientInterface );
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}



#endif