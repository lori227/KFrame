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
#include "KFKernel/KFKernelInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"

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

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        // 创建玩家
        virtual KFEntity* CreatePlayer( const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata );

        // 遍历玩家
        virtual KFEntity* FirstPlayer();
        virtual KFEntity* NextPlayer();

        // 查找玩家
        virtual KFEntity* FindPlayer( uint64 playerid );
        virtual KFEntity* FindPlayer( uint64 playerid, const char* function, uint32 line );

        // 删除玩家
        virtual void RemovePlayer( uint64 playerid );
        ////////////////////////////////////////////////////////////////////////////////
        virtual bool SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message );
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void AddInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveInitDataFunction( const std::string& moudle );

        virtual void AddUnInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveUnInitDataFunction( const std::string& moudle );

        virtual void AddRunDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveRunDataFunction( const std::string& moudle );

        virtual void AddAfterRunDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveAfterRunDataFunction( const std::string& moudle );

        virtual void AddResetFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveResetFunction( const std::string& moudle );

        virtual void AddEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveEnterFunction( const std::string& moudle );

        virtual void AddLeaveFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLeaveFunction( const std::string& moudle );

        virtual void AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveNewPlayerFunction( const std::string& moudle );

    private:
        // 初始化
        void InitPlayer( KFEntity* player );
        void UnInitPlayer( KFEntity* player );

        // 逻辑刷新
        void RunPlayer( KFEntity* player );
        void AfterRunPlayer( KFEntity* player );

        // 创建角色
        void OnEnterCreatePlayer( KFEntity* player, uint64 playerid );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;

        // 更新函数
        KFBind< std::string, const std::string&, KFEntityFunction  > _player_run_function;
        KFBind< std::string, const std::string&, KFEntityFunction  > _player_after_run_function;

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

        // 新玩家处理函数
        KFBind< std::string, const std::string&, KFEntityFunction > _new_player_function;
    };
}

#endif