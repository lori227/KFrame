#ifndef __KF_ROBOT_H__
#define __KF_ROBOT_H__

#include "KFrame.h"
#include "KFUtility/KFClock.h"
#include "KFNetwork/KFNetClientEngine.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFRobotConfig.h"
#include "KFMessage/KFMessageModule.h"
#include "KFRobotMessageModule.h"
#include "KFRobotPolicMgr.h"

namespace KFrame
{
    class KFFSM;
    class KFRobot
    {
    public:
        KFRobot();
        ~KFRobot();
        // 初始化
        void Initialize( uint32 robotid, KFNetClientEngine* netclient, KFComponent* kfcomponent );

        // 改变状态
        void ChangeState( uint32 state );

        // 改变状态 //外部配置调用这个接口
        void ChangeStateProxy();

        // 逻辑
        void Run();

        // 开启定时器
        void StartTimer( uint64 spacetime );

        // 停止定时器
        void StopTimer();

        // 响应定时器
        bool DoneTimer();

        // 连接到服务器
        void ConnectServer( const std::string& servertype, uint32 serverid, const std::string& name, const std::string& ip, uint32 port );

        // 断开连接
        void DisconnectServer( uint32 serverid );

        // 登录验证
        void OnConnectSendMessage();

        // 发送消息
        void SendNetMessage( uint32 msgid, google::protobuf::Message* message );

        //解析pb kfdata key
        void ParsePB2KFData( KFEntity* player, const KFMsg::PBObject* pbobject, KFData*& kfparent, uint64& key );
        void ParseUpdatePB2KFData( KFEntity* player, const KFMsg::PBObject* pbobject, KFData*& kfparent, uint64& key );

        // 创建角色
        void CreateRole();

        // 请求邮件数量
        void QueryMailNum();

        // 请求邮件列表
        void QueryMailInfo();

        // 查看邮件
        void ViewMail();

        // 领取邮件奖励
        void ReceiveMailReward();

        // 删除邮件
        void DelMail();

        // 进入聊天频道
        void EnterChat();

        // 发送聊天信息
        void SendChatMessage();

        // 离开聊天频道
        void LeaveChat();

        // 购买商品
        void BuyStore();

        // 赠送商品
        void GiveStore();

        // 添加好友
        void AddFriend();

        // 同意添加好友
        void AgreeInvite();

        // 删除好友
        void DelFriend();

        // 设置拒绝添加好友
        void SetRefuseFriendInvite();

        // 查询玩家基础信息
        void QueryBasic();

        // 修改名字
        void ChangeName();

        // 修改性别
        void ChangeSex();

        // 发送好友聊天
        void SendFriendChat();

        // 修改头像请求
        void ChangeIcon();

        // 修改头像框
        void ChangeIconBox();

        // 修改个性签名
        void ChangeMotto();

        // 修改模型
        void ChangeModel();

        // 修改默认时装
        void ChangeClothes();

        // 穿戴时装
        void DressClothes();

        // 查询访客信息
        void QueryGuest();

        // 领取活动奖励
        void RecvActivityReward();

        // 领取任务奖励
        void RecvTaskReward();

        // 领取成就奖励
        void RecvAchieveReward();

        // 领取礼包奖励
        void RecvGiftReward();

        // 敬酒
        void Toast();

        // 邀请组队
        void InviteTeam();

        // 同意组队邀请
        void AgreeInviteTeam();

        // 拒绝组队邀请
        void RefuseInviteTeam();

        // 拒绝并且5分钟内不接受邀请
        void RefuseMinuInviteTeam();

        // 离开组队
        void LeaveTeam();

        // 申请加入队伍
        void ApplyGroup();

        // 同意申请加入队伍
        void AgreeApplyTeam();

        // 拒绝申请加入队伍
        void RefuseApplyTeam();

        // 踢出队员
        void KickGroup();

        // GM加钱
        void AddMoney();

        // GM加钻石
        void AddDiamond();

        // GM加最近的人
        void AddRecent();

        // 增加心愿单
        void AddWishOrder();

        // 删除心愿单
        void DelWishOrder();

        // 修改心愿单可见状态
        void ModifyWishOrder();

        //////////////////////匹配相关///////////////////////////////
        // 开始单人匹配
        void SingleMatch();

        // 匹配
        void StartMatch( uint32 mode, bool group );
        // 取消匹配
        void CancleMatch();

        //////////////////////匹配相关///////////////////////////////
        // 查询玩家
        void QueryPlayer();

        // 查询商城版本号
        void QueryStoreVersion();

        // 查询商城信息
        void QueryStoreInfo();

        // 查询全服排行榜信息
        void QueryWholeRankList();

        // 查询好友排行榜信息
        void QueryFriendRankList();

    protected:

        // 定时器
        KFClockTimer _fsm_timer;

        // 客户端链接管理
        KFNetClientEngine* _net_client;

        // 属性组件
        KFComponent* _kf_component;
    public:


        // 状态机状态
        uint32 _state;
        // 下个状态 与上个状态时间间隔
        uint32 _loop_wait_times;

        // 状态机指针
        KFFSM* _kf_fsm;
        pNode _state_list;
    public:
        // 机器人id
        uint32 _robot_id;

        std::string _account;

        uint32 _playerid;
        uint32 _accountid;
        uint32 _matchid;

        // 目前只保存到player层
        //	KFMap< std::string, const std::string, KFComponentEx > _kf_component;

    public:
        std::string _connect_token;
        std::string _connect_ip;
        uint32  _connect_port;

    public:
        std::string _gate_server_id;
        std::string _gate_server_type;
        std::string _gate_ip;
        std::string _gate_port;
        std::string _gate_name;
        std::string _token;


    public:
        MapString _mail_num_arr;
    };
}

#endif