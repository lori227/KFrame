#ifndef __KF_PUBLIC_SHARD_MODULE_H__
#define __KF_PUBLIC_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    公共数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFPublicShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"

namespace KFrame
{
    class KFPublicShardModule : public KFPublicShardInterface
    {
    public:
        KFPublicShardModule() = default;
        ~KFPublicShardModule() = default;

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:

        // 连接route
        __KF_ROUTE_CONNECTION_FUNCTION__( OnConnectRouteCluster );
        ///////////////////////////////////////////////////////////
        // 更新公共数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateDataToPublicReq );

        // 查询基础数据
        __KF_MESSAGE_FUNCTION__( HandleQueryBasicToPublicReq );

    private:
        // 名字数据库
        KFRedisDriver* _name_redis_driver = nullptr;

        // 公共属性
        KFRedisDriver* _public_redis_driver = nullptr;
    };
}



#endif