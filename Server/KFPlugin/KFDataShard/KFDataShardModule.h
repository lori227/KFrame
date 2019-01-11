#ifndef __KF_DATA_SHARD_MODULE_H__
#define __KF_DATA_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFDataShardInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFDataKeeper
    {
    public:
        KFDataKeeper()
        {
            _zone_id = 0;
            _player_id = 0;
        }

        // 小区id
        uint32 _zone_id;

        // 玩家id
        uint64 _player_id;

        // 玩家数据
        KFMsg::PBObject _pb_object;
    };

    class KFDataShardModule : public KFDataShardInterface
    {
    public:
        KFDataShardModule() = default;
        ~KFDataShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 定时保存数据
        __KF_TIMER_FUNCTION__( OnTimerSaveDataKeeper );

        // route 连接
        __KF_ROUTE_CONNECTION_FUNCTION__( OnRouteConnection );

    protected:
        // 玩家登陆
        __KF_MESSAGE_FUNCTION__( HandleLoginLoadPlayerReq );

        // 保存数据
        __KF_MESSAGE_FUNCTION__( HandleSavePlayerReq );

        // 查询玩家属性
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerReq );

        // 处理设置名字
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameReq );

    protected:
        // 加载数据
        bool LoadPlayerData( uint32 zoneid, uint64 playerid, KFMsg::PBObject* pbobject );

        // 保存数据
        bool SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject );

        // 设置名字
        uint32 SetPlayerName( uint64 playerid, const std::string& oldname, const std::string& newname );

    private:
        // 数据保存
        KFMap< uint64, uint64, KFDataKeeper > _kf_data_keeper;

        // 名字数据库
        KFRedisDriver* _name_redis_driver{ nullptr };
    };
}



#endif