#ifndef __KF_DIR_SHARD_INTERFACE_H__
#define __KF_DIR_SHARD_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFDirShardInterface : public KFModule
    {
    public:
        // 分配玩家小区
        virtual StringMap AllocPlayerZone( uint32 zoneid ) = 0;

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zoneid, uint32 count ) = 0;

        // 获得world url
        virtual std::string GetWorldUrl( uint64 worldid ) = 0;
    };
    /////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_dir_shard, KFDirShardInterface );
    /////////////////////////////////////////////////////////////////////////////////
}


#endif