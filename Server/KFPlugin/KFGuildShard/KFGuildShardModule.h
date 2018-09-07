#ifndef __KF_GUILD_SHARD_MODULE_H__
#define __KF_GUILD_SHARD_MODULE_H__

/************************************************************************
//    @Module			:    帮派数据库模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFGuildShardInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFWorker/KFWorkerInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFOption/KFOptionInterface.h"
#include "KFSchedule/KFScheduleInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFClusterShard/KFClusterShardInterface.h"
#include "KFGuildShardConfig.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    class KFGuildShardModule : public KFGuildShardInterface
    {
    public:
        KFGuildShardModule();
        ~KFGuildShardModule();

        // 加载配置
        virtual void InitModule();

        // 初始化
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();

    protected:
        // 服务器发现客户端
        __KF_SERVER_DISCOVER_FUNCTION__( OnServerDiscoverClient );

    protected:
        // 帮派信息移除
        void DeleteGuild( KFEntity* kfentity );

        // 移除帮派的子属性
        __KF_REMOVE_DATA_FUNCTION__( OnRemoveDataCallBack );

        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );
        __KF_ADD_DATA_FUNCTION__( OnAddDataCallBack );
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateStringCallBack );

        // 同步更新属性到客户端
        void SendUpdateDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect );

        // 同步添加属性到客户端
        void SendAddDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect );

        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

    protected:
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildToShard );

        __KF_MESSAGE_FUNCTION__( HandleInviteGuildReq );

        __KF_MESSAGE_FUNCTION__( HandleUpdateMemberBasic );

        __KF_MESSAGE_FUNCTION__( HandleApplyGuildReq );

        __KF_MESSAGE_FUNCTION__( HandleExitGuildReq );

        __KF_MESSAGE_FUNCTION__( HandleTransferMasterReq );

        __KF_MESSAGE_FUNCTION__( HandleReviewApplyReq );

        __KF_MESSAGE_FUNCTION__( HandleDissolveGuildReq );

        __KF_MESSAGE_FUNCTION__( HandleModifyMedalReq );

        __KF_MESSAGE_FUNCTION__( HnadleQueryGuildListReq );

        // 定时刷新帮派(申请列表)
        __KF_TIMER_FUNCTION__( OnTimerRefreshGuild );

    protected:
        // 初始化帮派数据
        void LoadGuildData();

    private:
        // 初始化多个帮派完整数据
        void LoadMultiGuildData( std::list< MapString >& guildidlists );

        // 初始化单个帮派数据
        void LoadSingleGuildData( MapString& guilds );

        // 初始化单个帮派成员数据
        void LoadSingleGuildMember( KFData* kfdata, std::list< MapString >& memberlists );

    private:
        // 判断帮派名字是否存在
        bool IsRepeateName( const std::string& name );

        // 判断是否是帮派成员
        bool IsGuildMember( uint32 playerid );

        // 判断当前服务器中是否有该guildid
        bool IsValidGuildId( uint64 guildid );

        // 判断当前玩家是否是帮主
        bool IsMaster( uint32 playerid, uint64 guildid );

        // 判断是否是副帮主
        bool IsViceMaster( uint32 playerid, uint64 guildid );

        // 判断玩家是否在同一个帮派中
        bool IsSameGuild( uint32 playerid, uint32 desplayerid, uint64 guildid );

        // 判断玩家是否存在帮派申请列表中并且不超过一天
        bool IsInApplicanlist( uint64 guildid, uint32 playerid );

        // 将玩家加入申请列表
        bool JoinApplicanlist( uint64 guildid, uint32 playerid );

        // 清空申请列表
        bool ClearApplicanlist( uint64 guildid );

        // 刷新帮派申请列表
        void RefreshGuildApplicantlist( uint64 guildid );

        // 移除申请列表中的某个玩家
        bool RemoveApplicanlist( uint64 guildid, uint32 playerid );

        // 加入帮派
        bool JoinGuild( uint64 guildid, uint32 playerid, uint32 title = KFGuildShardEnum::GuildMember );

        // 获取玩家基础信息
        bool GetPlayBasic( uint32 playerid, MapString& values, VectorString& field );

        // 申请列表中的玩家全部加入到帮派中
        void MoveApplicantToGuild( uint64 guildid );

        // 清除离线玩家的guildid
        void  RemovePlayerGuildId( uint32 playerid );

        // 请求帮派列表
        bool QueryGuildListReq( uint32 cursor );

        // 请求指定帮派列表
        //void QueryGuildListPartReq( KFMsg::PBGuildIdArray& guilds );

    private:
        // 玩家申请列表 std::string转换成pb数据格式
        void GetGuildApplicantlist( uint64 guildid, KFMsg::PBApplicationlists& applylist );

        // 计算帮派排行榜的打榜分值
        uint64 CalcGuildRankDataScore( KFData* kfparent );

    private:

        //发送消息给同帮派成员
        /*
         *	@param [in] playerid:操纵者playerid
         *  @param [in] guild:帮派id
         *  @param [in] msgid:协议号id
         *  @param [in] message:消息包
         *  @param [in] offlineids:未收到消息的玩家id
         *  @param [in] containself:是否通知操纵者本身
        */
        bool SendMessageToGuildMember( uint32 playerid, uint64 guild, uint32 msgid, ::google::protobuf::Message* message, VectorString& offlineids, bool containself = true );

        bool SendPlayerGuildChangeToClient( const KFGuid& kfguid, uint32 playerid, uint64 guildid, uint32 code );

    private:
        // 帮派组件
        KFComponent* _kf_component;

        typedef std::function< bool()> KFLoadDataFunction;
        // 绑定加载数据模块函数
        KFBind< uint32, uint32, KFLoadDataFunction > _kf_load_function;
    };
}



#endif