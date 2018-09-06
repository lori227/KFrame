#ifndef __KF_PUBLIC_SHARD_MODULE_H__
#define __KF_PUBLIC_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    公共数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFrame.h"
#include "KFPublicShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFSchedule/KFScheduleInterface.h"

namespace KFrame
{
    class KFPublicShardModule : public KFPublicShardInterface
    {
    public:
        KFPublicShardModule();
        ~KFPublicShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        // 更新公共数据
        __KF_MESSAGE_FUNCTION__( HandleUpdatePublicDataReq );

        // 查询基础数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicDataReq );

        // 处理创建角色
        __KF_MESSAGE_FUNCTION__( HandleCreateRoleReq );

        // 处理设置名字
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameReq );

        // 处理更新访客列表请求
        __KF_MESSAGE_FUNCTION__( HandleUpdateGuestListReq );

        // 处理查询访客请求
        __KF_MESSAGE_FUNCTION__( HandleQueryGuestReq );


    private:
        // 设置玩家名字
        uint32 ProcessSetPlayerName( uint32 playerid, const std::string& oldname, const std::string& newname );
    };
}



#endif