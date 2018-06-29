#ifndef __KF_RANK_SHARD_CONFIG_H__
#define __KF_RANK_SHARD_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRankShardConfig : public KFConfig, public KFSingleton< KFRankShardConfig >
    {
    public:
        KFRankShardConfig();
        ~KFRankShardConfig();

        // 加载配置
        bool LoadConfig( const char* file );

        // 获得当前zone
        const std::set< uint32 >* FindZoneId( uint32 serverid );

    protected:
        void AddServerZone( uint32 zoneid, uint32 serverid );

    public:
        // 分区服务器列表
        std::map< uint32, std::set< uint32 > > _server_zone_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_rank_shard_config = KFRankShardConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif