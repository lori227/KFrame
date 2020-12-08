#ifndef __KF_BASIC_SHARD_MODULE_H__
#define __KF_BASIC_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    公共数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-11-30
************************************************************************/

#include "KFBasicShardInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFBasicDatabase/KFBasicDatabaseInterface.h"

namespace KFrame
{
    class KFBasicShardModule : public KFBasicShardInterface
    {
    public:
        KFBasicShardModule() = default;
        ~KFBasicShardModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        ///////////////////////////////////////////////////////////
        // 更新基础数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateIntValueToBasicReq, KFMsg::S2SUpdateIntValueToBasicReq );
        __KF_MESSAGE_FUNCTION__( HandleUpdateStrValueToBasicReq, KFMsg::S2SUpdateStrValueToBasicReq );

        // 清空在线数据
        __KF_MESSAGE_FUNCTION__( HandleClearOnlineToBasicReq, KFMsg::S2SClearOnlineToBasicReq );

        // 查询基础数据
        __KF_MESSAGE_FUNCTION__( HandleQueryAttributeToBasicReq, KFMsg::S2SQueryAttributeToBasicReq );

        // 处理设置名字
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameToBasicReq, KFMsg::S2SSetPlayerNameToBasicReq );
    };
}



#endif