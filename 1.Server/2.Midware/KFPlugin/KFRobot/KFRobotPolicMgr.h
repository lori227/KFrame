#ifndef __KF_ROBOT_POLIC_MGR_H__
#define __KF_ROBOT_POLIC_MGR_H__

#include "KFrame.h"
#include "KFKernel/KFKernelInterface.h"
namespace KFrame
{
    // 商城
    typedef struct StoreSetting
    {
        std::string buytype;
        uint32 shopid;
        uint32 num;
    } T_StoreSetting, *pStoreSetting;

    // 活动
    typedef struct ActivitySetting
    {
        uint32 type;
        uint32 id;
    } T_ActivitySetting, *pActivitySetting;

    // 存储玩家状态链表
    typedef struct  Node
    {
        uint32 _state;
        Node* prev;
        Node* next;

    } T_Node, *pNode;

    namespace KFRObotEnum
    {
        enum MatchIdEmun
        {
            Single_Match = 1,// 单人组队模式
            Double_Match = 2,// 双人组队模式
            Penta_Match = 3,//	四人组队模式
        };
    }

    // 帮派列表
    typedef struct GuildListData
    {
        GuildListData()
        {
            guildid = 0u;
            guildname = _invalid_str;
            level = _invalid_int;
            manifesto = _invalid_str;
            medal = _invalid_int;
            totalactiveness = 0u;
            masterid = _invalid_int;
        }

        uint64			guildid;			// 帮派id
        std::string		guildname;			// 帮派名字
        uint32			level;				// 帮派等级
        std::string		manifesto;			// 帮派宣言
        uint32			medal;				// 帮派勋章
        uint64			totalactiveness;	// 帮派总活跃
        uint32			masterid;			// 帮主id
    } T_GuildListData, *pGuildListData;


    class KFRobotPolicMgr : public KFSingleton< KFRobotPolicMgr >
    {
    public:
        KFRobotPolicMgr();
        ~KFRobotPolicMgr();
        void Init();
        void PushSortRoleInfo( uint32 playerid );
        void AddRoleInfo( uint32 playerid, KFEntity* player );
        void RemoveRole( uint32 playerid );
        KFEntity* FindRoleById( uint32 playerid );

        void InitStateList( std::string& oddlist, std::string& evenlist );

        // 商城
        bool GetStoreSetting( std::string& buytype, uint32& shopid, uint32& num, int cursor );
        inline uint32 GetStoreSize()
        {
            return _storeSettings.size();
        }

        // 获取相对应的其他roleid
        uint32 GetFetterRole( uint32 playerid );
        bool GetAllFetterRole( std::vector<uint32>& roleids, uint32 playerid );

        // 根据playerid 获取状态链表
        pNode GetStateListById( uint32 playerid );

        // 根据playerid获取角色名字
        void GetPlayerName( uint32 playerid, std::string& name );


    private:
        void InitOddList( std::string& oddlist );
        void InitEvenList( std::string& evenlist );

    private:
        // 商城配置
        std::vector<pStoreSetting> _storeSettings;

        //活动配置
        std::vector<pActivitySetting> _activitySettings;

        typedef std::map<uint32, uint32> t_uintmap;
        t_uintmap _oddplayer;
        t_uintmap _evenplayer;

        uint32 _oddbase;
        uint32 _evenbase;

        typedef std::map<uint32, KFEntity*> t_player_map;
        std::map<uint32, KFEntity*> _players;
        pNode _oddStateList;
        pNode _evenStateList;

    };
}

#endif//__KF_ROBOT_POLIC_MGR_H__
