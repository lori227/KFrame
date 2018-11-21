#ifndef __KF_BATTLE_MANAGE_H__
#define __KF_BATTLE_MANAGE_H__

#include "KFrame.h"
#include "KFRedis/KFRedisInterface.h"

namespace KFrame
{
    class KFBattleServer
    {
    public:
        KFBattleServer();
        ~KFBattleServer() = default;

        // 是否有效
        bool IsValid() const;

        // 充值
        void Reset();

    public:
        uint32 _server_id;		// 服务器id
        uint32 _proxy_id;		// 连接的代理服务器id
        std::string _ip;		// ip
        uint32 _port;			// port
        std::string _version;	// 版本号
    };

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    // 战斗房间分配管理
    class KFBattleManage : public KFSingleton< KFBattleManage >
    {
    public:
        KFBattleManage();

        // 初始化
        void Initialize();

        // 注册战场
        void RegisterBattleServer( uint32 serverid, uint32 proxyid, const std::string& ip, uint32 port, const std::string& version );

        // 删除战场
        void UnRegisterBattleServer( uint32 serverid );

        // 分配战场
        void AllocBattleServer( uint32 battleserverid, const std::string& version, KFBattleServer* battleserver );

        // 分配一个战场
        void RemoveBattleServer( uint32 serverid, const std::string& version );

        // 释放战斗服务器
        void FreeBattleServer( uint32 serverid, const std::string& version );

    private:
        KFRedisDriver* _battle_redis_driver;
    };

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    static auto _kf_battle_manage = KFBattleManage::Instance();
}

#endif