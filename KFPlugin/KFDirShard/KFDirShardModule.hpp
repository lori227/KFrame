#ifndef __KF_DIR_SHARD_MODULE_H__
#define __KF_DIR_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    目录模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFDirShardInterface.h"
#include "KFDirShardRedis.hpp"
#include "KFDirShardMongo.hpp"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFDirShardModule : public KFDirShardInterface
    {
    public:
        KFDirShardModule() = default;
        ~KFDirShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 分配玩家小区
        virtual StringMap AllocPlayerZone( uint32 zoneid );

        // 小区人数负载均衡
        virtual bool ZoneBalance( uint32 zoneid, uint32 count );

        // 获得world url
        virtual std::string GetWorldUrl( uint64 worldid );
    protected:
        // 小区信息注册
        __KF_HTTP_FUNCTION__( HandleZoneRegister );

        // 小区信息更新
        __KF_HTTP_FUNCTION__( HandleZoneUpdate );

        // 查询小区列表
        __KF_HTTP_FUNCTION__( HandleQueryZoneList );

        // 查询小区ip
        __KF_HTTP_FUNCTION__( HandleQueryZoneIp );

        // 小区人数更新
        __KF_HTTP_FUNCTION__( HandleZoneBalance );

        // 推荐小区
        __KF_HTTP_FUNCTION__( HandleZoneRecommend );

        // world的http地址
        __KF_HTTP_FUNCTION__( HandleWorldRegister );

    protected:
        KFDirShardLogic* _dir_shard_logic = nullptr;
    };
}



#endif