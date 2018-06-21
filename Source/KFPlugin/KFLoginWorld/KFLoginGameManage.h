#ifndef __KF_LOGIN_GAME_MANAGE_H__
#define __KF_LOGIN_GAME_MANAGE_H__

#include "KFrame.h"
#include "KFConHash/KFConHash.h"

namespace KFrame
{
	class KFLoginGame
	{
	public:
		KFLoginGame()
		{
			_game_id = 0;
			_client_count = 0;
		}

		// id
		uint32 _game_id;
				
		// 当前连接的客户端
		uint32 _client_count;
	};

	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	// 游戏管理器
	class KFLoginGameManage : public KFSingleton< KFLoginGameManage >
	{
	public:
		KFLoginGameManage();
		~KFLoginGameManage();
		
		// 添加登录节点
		KFLoginGame* AddLoginGame( uint32 handleid );

		// 删除登录节点
		bool RemoveLoginGame( uint32 handleid );

		// 更新登录节点
		void UpdateLoginGame( uint32 handleid, uint32 clientcount );
		
		// 查找登录节点
		KFLoginGame* FindLoginGame( uint32 handleid );

		// 选择登录节点
		KFLoginGame* SelectMinLoginGame();
		KFLoginGame* HashLoginGame( uint32 playerid );

	public:
		// 登录节点列表
		KFConHash _kf_conhash;
		KFMap< uint32, uint32, KFLoginGame > _kf_game_list;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	static auto _kf_login_game_manage = KFLoginGameManage::Instance();
	///////////////////////////////////////////////////////////////////////////////////////////////
}



#endif