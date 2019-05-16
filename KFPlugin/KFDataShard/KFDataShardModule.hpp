#ifndef __KF_DATA_SHARD_MODULE_H__
#define __KF_DATA_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

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

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 定时保存数据
        __KF_TIMER_FUNCTION__( OnTimerSaveDataKeeper );

    protected:
        // 玩家登陆
        __KF_MESSAGE_FUNCTION__( HandleLoadPlayerToDataReq );

        // 保存数据
        __KF_MESSAGE_FUNCTION__( HandleSavePlayerToDataReq );

        // 查询玩家属性
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerToDataReq );


    protected:
        // 加载数据
        bool LoadPlayerData( uint32 zoneid, uint64 playerid, KFMsg::PBObject* pbobject );

        // 保存数据
        bool SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject );
    private:
        // 数据保存
        KFHashMap< uint64, uint64, KFDataKeeper > _kf_data_keeper;
    };
}



#endif