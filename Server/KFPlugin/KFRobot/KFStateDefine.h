#ifndef __KF_STATE_DEFINE__
#define __KF_STATE_DEFINE__

namespace KFrame
{
	enum RobotStateEnum
	{
		IdleState = 0,	// 空闲状态
		AuthState = 1,	// 认证状态
		LoginVerify = 2,// 登录验证状态
		LoginGame = 3,	// 登录游戏
		CreateRole = 4,	// 创建角色
		QueryMailNum = 5,//查询邮件数量
		QueryMail = 6,	// 查询邮件
		ViewMail = 7,	// 查看邮件
		ReciveMailReward = 8, //领取邮件奖励
		DelMail = 9,	// 删除邮件
		EnterChat = 10,	// 进入聊天
		SendChat = 11,	// 发送聊天信息
		LeaveChat = 12,	// 离开聊天频道
		BuyStore = 13,	// 商城购买
		GiveStore = 14,	// 商城赠送
		AddFriend = 15,	// 添加好友
		DelFriend = 16,	// 删除好友
		RefuseFriend = 17, //拒绝好友申请
		QueryBasic = 18,// 查询基础玩家信息
		ChangeName = 19,// 修改名字
		ChangeSex = 20,	// 修改性别
		SendFriendChat = 21,//发送好友聊天
		ChangeIcon = 22,// 修改头像路径
		ChangeIconBox = 23,// 修改头像框
		ChangeMotto = 24, // 修改个性签名
		ChangeModel = 25, // 修改模型
		ChangeClothes = 26,// 设置模型的默认时装id
		DressClothes = 27, // 穿戴时装
		QueryGuest = 28,  // 查询访客信息
		RecvActivityReward = 29,//领取活动奖励
		RecvTaskReward = 30, // 领取任务奖励
		RecvAchieveReward = 31,// 领取成就奖励
		RecvGiftReward = 32, // 领取礼包奖励
		Toask = 33,			// 敬酒

		QueryPlayer = 45,	//查询玩家
		QueryStoreVersion = 46,//请求商城版本号
		QueryStoreInfo = 47, //请求商城热更信息详情


		InviteTeam = 34,// 邀请组队
		AgreeInviteTeam = 35,// 同意邀请组队
		RefuseInviteTeam = 36, // 拒绝邀请组队
		RefuseMinuInviteTeam = 37,//拒绝并且5分钟内不接受邀请
		LeaveTeam = 38,		// 离开队伍
		ApplyGroup = 39,	// 申请加入队伍
		AgreeApplyTeam = 40, // 同意申请组队
		RefuseApplyTeam = 41, // 拒绝申请组队
		KickGroup = 42,	//踢出队员

		SingleMatch = 43,		// 单人匹配不组队
		CancelMatch = 44,		// 取消匹配
		DoubleMatchGroup = 48,	// 双人匹配允许组队
		DoubleMatch = 49,		// 双人匹配不允许组队
		PentaMatchGroup = 50,	// 四人匹配允许组队
		PentaMatch = 51,		// 四人匹配不允许组队

		// 心愿单
		AddWishOrder = 100,		// 增加心愿单
		// 删除心愿单
		DelWishOrder = 101,		// 删除心愿单
		// 修改心愿单状态
		ModifyWishOrder = 102,	// 修改心愿单

		// 排行榜
		QueryWholeRankList = 103,	// 查询全服排行榜
		QueryFriendRankList = 104,	// 查询好友排行榜


		GMAddMoney = 1000,		// 加钱
		GMAddDiamond = 1001,	// 加钻石

		// 机器人空闲状态
		PauseState = 9998,		// 机器人停止切换状态 
	};
}

#endif