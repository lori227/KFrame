#ifndef __KF_GUILD_CLIENT_MODULE_H__
#define __KF_GUILD_CLIENT_MODULE_H__

/************************************************************************
//    @Module			:    帮派数据模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2018-5-16
************************************************************************/

#include "KFrame.h"
#include "KFGuildClientInterface.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFClusterClient/KFClusterClientInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFMailClient/KFMailClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFSchedule/KFScheduleInterface.h"

namespace KFrame
{
    class KFGuildClientModule : public KFGuildClientInterface
    {
    public:
        KFGuildClientModule();
        ~KFGuildClientModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////
        // 发送消息到Guild
        virtual bool SendMessageToGuild( uint64 guildid, uint32 msgid, ::google::protobuf::Message* message );

        virtual bool SendMessageToGuild( uint32 msgid, ::google::protobuf::Message* message );

        virtual void UpdateMemberBasic( uint32 playerid, uint64 guildid, MapString& basics );

    protected:
        // 进入游戏查看帮派信息
        void OnEnterQueryGuild( KFEntity* player );

        void OnEnterUpdateGuildData( KFEntity* player );
        void OnLeaveUpdateGuildData( KFEntity* player );
        bool UpdatePublicData( uint64 guildid, uint32 playerid, const MapString& values );
    protected:
        // 处理创建帮派请求
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildReq );

        // 处理创建帮派反馈
        __KF_MESSAGE_FUNCTION__( HandleCreateGuildAck );

        // 处理邀请玩家邀请加入帮派
        __KF_MESSAGE_FUNCTION__( HnadleInviteGuildReq );

        // 处理邀请玩家邀请加入帮派反馈
        __KF_MESSAGE_FUNCTION__( HnadleInviteGuildAck );

        // 处理玩家申请加入帮派
        __KF_MESSAGE_FUNCTION__( HandleApplyGuildReq );

        // 处理玩家申请加入帮派反馈
        __KF_MESSAGE_FUNCTION__( HandleApplyGuildAck );

        // 处理玩家帮派变化
        __KF_MESSAGE_FUNCTION__( HandlePlayerGuildChangeReq );

        // 玩家退出帮派
        __KF_MESSAGE_FUNCTION__( HandleExitGuildReq );

        // 转让帮主
        __KF_MESSAGE_FUNCTION__( HandleTransferGuildReq );

        // 帮派审批
        __KF_MESSAGE_FUNCTION__( HandleReviewApplyReq );

        // 解散帮派
        __KF_MESSAGE_FUNCTION__( HandleDissolveGuildReq );

        // 修改帮派徽章
        __KF_MESSAGE_FUNCTION__( HandleModifyMedalReq );

        // 请求帮派列表
        __KF_MESSAGE_FUNCTION__( HandleQueryGuildListReq );

        // 请求踢掉玩家
        __KF_MESSAGE_FUNCTION__( HandleKickMemberReq );

        // 请求帮派升级
        __KF_MESSAGE_FUNCTION__( HandleUpgradeGuildReq );

        // 请求帮派任命
        __KF_MESSAGE_FUNCTION__( HandleAppointGuildMemberReq );

        // 请求帮派数据返回
        __KF_MESSAGE_FUNCTION__( HandleQueryGuildAck );

        // 请求帮派升级返回
        __KF_MESSAGE_FUNCTION__( HandleUpgradeGuildAck );

        // 搜索帮派
        __KF_MESSAGE_FUNCTION__( HandleSearchGuildByNameReq );

        // 设置帮派管理开关
        __KF_MESSAGE_FUNCTION__( HandleSetSwitchReq );

        // 帮派数据更新
        __KF_MESSAGE_FUNCTION__( HandleUpDateGuildAck );

        // 帮派数据增加
        __KF_MESSAGE_FUNCTION__( HandleAddGuildAck );

        // 帮派数据移除
        __KF_MESSAGE_FUNCTION__( HandleRemoveGuildAck );

        // 请求帮派id返回
        __KF_MESSAGE_FUNCTION__( HandleLoginQueryGuildIdAck );

        // 查询帮派日志
        __KF_MESSAGE_FUNCTION__( HandleQueryGuildLogReq );

    protected:
        // 更新属性回调
        __KF_UPDATE_DATA_FUNCTION__( OnUpdateDataCallBack );

        __KF_UPDATE_STRING_FUNCTION__( OnUpdateStringCallBack );

        // 帮派申请列表更新
        __KF_UPDATE_STRING_FUNCTION__( OnGuildApplicantUpdateCallBack );
    private:

        // 玩家申请列表 std::string转换成pb数据格式
        bool GetGuildApplicantlist( const std::string& strapplicantlist, KFMsg::PBApplicationlists& applylist );

        // 获取玩家申请列表
        KFData* GetGuildApplicantFormProto( const KFDataSetting* datasetting, const KFMsg::PBApplicationlist* applylist );


        // 解析update guild消息
        void ParseUpdateGuildFromProto( KFEntity* player, KFData* kfparent, const KFMsg::PBObject* proto );

        // 解析Add guild消息
        void ParseAddGuildFromProto( KFEntity* player, KFData* kfparent, KFData* kfdata, const KFMsg::PBObject* proto );

        // 解析remove guild消息
        void ParseRemoveGuildProto( KFEntity* player, KFData* kfparent, const KFMsg::PBObject* proto );

        // 开启帮派申请列表超时定时器
        void CheckStartApplicantlistTimer( KFEntity* player );

        // 删除过期申请列表
        void RemoveVaildApplicantList( KFEntity* player );

        // 删除过期邀请列表
        void RemoveVaildInviteList( KFEntity* player );

        // 是否含有空格或者%号
        bool IsValidName( const std::string& name );

        // 检测申请列表\邀请列表是否过期
        __KF_TIMER_FUNCTION__( OnTimerCheckApplicantlistValidTime );


    private:
        // 组件
        KFComponent* _kf_component;
    };
}

#endif