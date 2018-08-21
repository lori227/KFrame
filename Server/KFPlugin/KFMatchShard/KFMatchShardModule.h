#ifndef __KF_MATCH_SHARD_MODULE_H__
#define __KF_MATCH_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    匹配房间逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-12-20
************************************************************************/

#include "KFrame.h"
#include "KFMatchShardInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFMatchQueue.h"

namespace KFrame
{
    class KFMatchShardModule : public KFMatchShardInterface
    {
    public:
        KFMatchShardModule();
        ~KFMatchShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void Run();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 处理匹配请求
        __KF_MESSAGE_FUNCTION__( HandleMatchToShardReq );

        // 查询匹配
        __KF_MESSAGE_FUNCTION__( HandleQueryRoomToMatchShardReq );

        // 取消匹配
        __KF_MESSAGE_FUNCTION__( HandleCancelMatchToShardReq );

        // 取消匹配
        __KF_MESSAGE_FUNCTION__( HandleCancelMatchToMatchShardAck );

        // 创建房间回馈
        __KF_MESSAGE_FUNCTION__( HandleCreateRoomToMatchShardAck );

        // 战场开启
        __KF_MESSAGE_FUNCTION__( HandleOpenRoomToMatchShardReq );

        // 战场房间正式开始
        __KF_MESSAGE_FUNCTION__( HandleTellRoomStartToMatchShardReq );

        // 阵营加入房间
        __KF_MESSAGE_FUNCTION__( HandleAddCampToMatchShardAck );

        // 玩家离开战场
        __KF_MESSAGE_FUNCTION__( HandlePlayerLeaveRoomToMatchShardReq );

        // 战场关闭
        __KF_MESSAGE_FUNCTION__( HandleTellRoomCloseToMatchShardReq );

    protected:
        // Match Master连接成功
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectMatchMaster );

    protected:
        // 逻辑run

    private:
        // 匹配模式列表
        KFMap< uint32, uint32, KFMatchQueue > _kf_match_queue;
    };
}



#endif