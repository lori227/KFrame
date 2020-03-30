#ifndef __KF_PAL_SHARD_MODULE_H__
#define __KF_PAL_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    充值模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-8-23
************************************************************************/

#include "KFPayShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFChannel/KFChannelInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

namespace KFrame
{
    class KFPayShardModule : public KFPayShardInterface
    {
    public:
        KFPayShardModule() = default;
        ~KFPayShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 申请订单
        __KF_HTTP_FUNCTION__( HandleApplyOrder );

        // 删除订单
        __KF_HTTP_FUNCTION__( HandleRemoveOrder );

        // 查询充值
        __KF_HTTP_FUNCTION__( HandleQueryPay );

        // 完成充值
        __KF_HTTP_FUNCTION__( HandleFinishPay );

        // 内部测试充值回调
        __KF_HTTP_FUNCTION__( HandleInternalPay );

    protected:

    };
}



#endif