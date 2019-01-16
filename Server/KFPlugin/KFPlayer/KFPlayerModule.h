#ifndef __KF_PLAYER_MODULE_H__
#define __KF_PLAYER_MODULE_H__

/************************************************************************
//    @Module			:    游戏玩家基础功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/

#include "KFPlayerInterface.h"
#include "KFProtocol/KFProtocol.h"
#include "KFGame/KFGameInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFDataClient/KFDataClientInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFPlayerModule : public KFPlayerInterface
    {
    public:
        KFPlayerModule() = default;
        ~KFPlayerModule() = default;

        // 刷新
        virtual void BeforeRun();
        virtual void OnceRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 查找玩家
        virtual KFEntity* FindPlayer( uint64 playerid );
        virtual KFEntity* FindPlayer( uint64 playerid, const char* function, uint32 line );
        ////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        virtual bool LoadPlayer( const KFMsg::PBLoginData* pblogin );

        // 保存玩家
        virtual void SavePlayer( KFEntity* player );

        // 查询玩家
        virtual bool QueryPlayer( uint64 sendid, uint64 playerid );

        // 遍历玩家
        virtual KFEntity* FirstPlayer();
        virtual KFEntity* NextPlayer();

        // 删除玩家
        virtual void RemovePlayer( uint64 playerid );
        ////////////////////////////////////////////////////////////////////////////////
        // 设置名字
        virtual bool SetName( uint64 playerid, const std::string& oldname, const std::string& newname, uint64 itemguid );

        // 判断操作频率
        virtual bool CheckOperateFrequently( KFEntity* player, uint32 time );

    protected:
        virtual void SetAfterLoadFunction( KFLoadPlayerFunction& function );
        virtual void SetAfterQueryFunction( KFQueryPlayerFunction& function );
        virtual void SetAfterSetNameFunction( KFAfterSetNameFunction& function );

        virtual void AddInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveInitDataFunction( const std::string& moudle );

        virtual void AddUnInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveUnInitDataFunction( const std::string& moudle );

        virtual void AddRunDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveRunDataFunction( const std::string& moudle );

        virtual void AddResetFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveResetFunction( const std::string& moudle );

        virtual void AddEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveEnterFunction( const std::string& moudle );

        virtual void AddLeaveFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLeaveFunction( const std::string& moudle );

        virtual void AddLogoutFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLogoutFunction( const std::string& moudle );

        virtual void AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveNewPlayerFunction( const std::string& moudle );

    private:
        // 初始化
        void InitPlayer( KFEntity* player );
        void UnInitPlayer( KFEntity* player );

        // 逻辑刷新
        void RunPlayer( KFEntity* player );

        // 创建角色
        KFEntity* CreatePlayer( const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata );

        // 创建角色
        void OnEnterCreatePlayer( KFEntity* player, uint64 playerid );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 加载玩家数据
        void OnLoadPlayerData( uint32 result, const KFMsg::PBLoginData* pblogin, KFMsg::PBObject* pbplayerdata );

        // 查询玩家回调
        void OnQueryPlayerData( uint32 result, uint64 sendid, KFMsg::PBObject* pbplayerdata );

        // 设置名字回调
        void OnSetName( uint32 result, uint64 playerid, const std::string& name, uint64 itemguid );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新属性到客户端
        void SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步添加属性到客户端
        void SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 显示添加奖励消息
        void SendRewardElementToClient( KFEntity* player, const std::string& reward );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 玩家组件
        KFComponent* _kf_component{ nullptr };

        // 玩家数据
        KFData* _query_player_data{ nullptr };

        // 加载玩家函数
        KFLoadPlayerFunction _after_load_function{ nullptr };
        KFQueryPlayerFunction _after_query_function{ nullptr };
        KFAfterSetNameFunction _after_set_name_function{ nullptr };

        // 更新函数
        KFBind< std::string, const std::string&, KFEntityFunction  > _player_run_function;

        // 初始化函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_init_function;

        // 卸载函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_uninit_function;

        // 重置函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_reset_function;

        // 登录函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_enter_function;

        // 离开函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_leave_function;

        // 登出函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_logout_function;

        // 新玩家处理函数
        KFBind< std::string, const std::string&, KFEntityFunction > _new_player_function;

    };
}

#endif