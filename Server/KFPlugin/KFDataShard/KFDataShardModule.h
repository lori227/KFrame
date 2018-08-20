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
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"

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
        uint32 _player_id;

        // 玩家数据
        KFMsg::PBObject _pb_object;
    };

    class KFDataShardModule : public KFDataShardInterface
    {
    public:
        KFDataShardModule();
        ~KFDataShardModule();

        // 加载配置
        virtual void InitModule();

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
        __KF_MESSAGE_FUNCTION__( HandleLoginLoadPlayerReq );

        // 保存数据
        __KF_MESSAGE_FUNCTION__( HandleSavePlayerReq );

        // 删除数据
        __KF_MESSAGE_FUNCTION__( HandleDeletePlayerReq );

        // 查询玩家属性
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerReq );

    protected:
        // 加载数据
        bool LoadPlayerData( uint32 zoneid, uint32 id, KFMsg::PBObject* pbobject );

        // 保存数据
        bool SavePlayerData( uint32 zoneid, uint32 id, const KFMsg::PBObject* pbobject );

        // 删除数据
        void DeletePlayerData( uint32 zoneid, uint32 id );

    private:
        // 数据保存
        KFMap< uint32, uint32, KFDataKeeper > _kf_data_keeper;
    };
}



#endif