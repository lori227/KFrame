#ifndef __KF_MATCH_PROXY_MODULE_H__
#define __KF_MATCH_PROXY_MODULE_H__

/************************************************************************
//    @Module			:    陪陪逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-20
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFMatchProxyInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFClusterProxy/KFClusterProxyInterface.h"

namespace KFrame
{
    class KFMatchProxyModule : public KFMatchProxyInterface
    {
    public:
        KFMatchProxyModule();
        ~KFMatchProxyModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 注册匹配信息
        __KF_MESSAGE_FUNCTION__( HandleRegisterMatchReq );

        // 处理匹配
        __KF_MESSAGE_FUNCTION__( HandleMatchToProxyReq );

        // 查询匹配
        __KF_MESSAGE_FUNCTION__( HandleQueryMatchRoomReq );

        // 取消匹配
        __KF_MESSAGE_FUNCTION__( HandleCancelMatchToProxyReq );

    protected:
        // 丢失连接
        __KF_CLIENT_LOST_FUNCTION__( OnClientLostMatchShard );

    private:
        // 查找一个匹配服务器
        uint32 FindMatchShard( uint32 matchid, uint32 playerid );

    private:
        // 匹配shard列表
        KFMap< uint32, uint32, KFConHash > _match_shard;
    };
}



#endif