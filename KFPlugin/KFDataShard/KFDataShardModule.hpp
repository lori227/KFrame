#ifndef __KF_DATA_SHARD_MODULE_H__
#define __KF_DATA_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFDataShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMySQL/KFMySQLInterface.h"
#include "KFMongo/KFMongoInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFDataExecute.hpp"

namespace KFrame
{
    class KFDataShardModule : public KFDataShardInterface
    {
    public:
        KFDataShardModule() = default;
        ~KFDataShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void PrepareRun();

        // 逻辑
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
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
        bool SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject, uint32 saveflag );
    private:
        // 数据库逻辑
        KFMap< uint32, KFDataExecute > _data_execute;
    };
}



#endif