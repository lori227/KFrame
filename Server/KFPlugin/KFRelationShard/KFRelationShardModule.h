#ifndef __KF_RELATION_SHARD_MODULE_H__
#define __KF_RELATION_SHARD_MODULE_H__

/************************************************************************
//    @Moudle			:    好友逻辑模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-3-8
************************************************************************/

#include "KFrame.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRelationShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFDisplay/KFDisplayInterface.h"

namespace KFrame
{
    class KFRelationShardModule : public KFRelationShardInterface
    {
    public:
        KFRelationShardModule();
        ~KFRelationShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        // 查询玩家好友信息
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendReq );

        // 查询好友申请
        __KF_MESSAGE_FUNCTION__( HandleQueryFriendInviteReq );

        // 申请添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendInviteReq );

        // 删除好友邀请
        __KF_MESSAGE_FUNCTION__( HandleDelInviteReq );

        // 添加好友
        __KF_MESSAGE_FUNCTION__( HandleAddFriendReq );

        // 删除好友
        __KF_MESSAGE_FUNCTION__( HandleDelFriendReq );

        // 更新好友度
        __KF_MESSAGE_FUNCTION__( HandleUpdateFriendLinessReq );

        // 添加最近的人
        __KF_MESSAGE_FUNCTION__( HandleAddBattleFriendDataReq );

        // 查询最近的玩家列表
        __KF_MESSAGE_FUNCTION__( HandleQueryRecentListReq );

    protected:
        // 信息转换成好友信息
        void MapStringToPBPlayer( MapString& values, uint32 friendid, KFMsg::PBFriend* pbfriend );
        void MapStringToPBFriend( MapString& values, KFMsg::PBFriend* pbfriend, bool newadd );
        void MapStringToPBBasic( MapString& values, KFMsg::PBStrings* pbstrings );

        // 格式化好友key
        std::string FormatFriendKey( const std::string& key, uint32 firstid, uint32 secondid );
        std::string FormatFriendLimitKey( uint32 firstid, uint32 secondid, uint32 type );

        // 发送添加好友消息
        void SendAddFriendToClient( uint32 serverid, MapString& values, uint32 friendid, uint32 playerid );

        // 发送更新好友度
        void SendAddFriendLinessToClient( uint32 selfid, uint32 targetid, uint32 friendliness );

        // 计划清理数据库
        void OnScheduleClearFriendLiness( uint32 id, const char* data, uint32 size );

        // 是否是好友成员信息
        bool IsFriend( uint32 playerid, uint32 targetid );

        // 是否已经在最近游戏列表
        bool IsRecentFriend( uint32 playerid, uint32 targetid );

        // 删除已经存在的最近游戏列表
        void DelRecentListToDB( uint32 playerid );

        // 保存最近游戏列表
        void SaveRecentListToDB( uint32 playerid, VectorString& uidinfos );

        // 通知客户端操作最近游戏列表
        void SendRecentListToClient( uint32 selfid, uint32 operate, VectorString& uidinfos );

        // 更新好友度
        void UpdateFriendLiness( uint32 selfplayerid, uint32 targetplayerid, uint32 addvalue, uint32 type );

        // 处理战斗结算好感度
        void HandleBattleFrinedLiness( const char* data, uint32 length );

    private:
        std::set< uint32 > _add_uids;
        std::set< uint32 > _del_uids;
    };
}



#endif