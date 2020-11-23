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
#include "KFMessage/KFMessageInterface.h"
#include "KFCommand/KFCommandInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFTcpServer/KFTcpServerInterface.h"
#include "KFConfig/KFPlayerConfig.hpp"

namespace KFrame
{
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
        virtual KFEntity* Login( const KFMsg::PBLoginData* pblogin, const KFMsg::PBObject* pbplayerdata );
        virtual KFEntity* ReLogin( uint64 playerid, uint64 gateid );

        // 查找玩家
        virtual KFEntity* FindPlayer( uint64 playerid );
        virtual KFEntity* FindPlayer( uint64 playerid, const char* function, uint32 line );

        // 删除玩家
        virtual void RemovePlayer();
        virtual void RemovePlayer( uint64 playerid );
        virtual void RemovePlayer( KFEntity* player );
        ////////////////////////////////////////////////////////////////////////////////
        virtual bool SendToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message, uint32 delay = 0 );
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

        virtual void AddBeforeEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveBeforeEnterFunction( const std::string& moudle );

        virtual void AddEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveEnterFunction( const std::string& moudle );

        virtual void AddAfterEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveAfterEnterFunction( const std::string& moudle );

        virtual void AddLeaveFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLeaveFunction( const std::string& moudle );

        virtual void AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveNewPlayerFunction( const std::string& moudle );

        virtual void AddCreateRoleFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveCreateRoleFunction( const std::string& moudle );

    protected:
        // 初始化
        void InitPlayer( KFEntity* player );
        void UnInitPlayer( KFEntity* player );

        // 逻辑刷新
        void RunPlayer( KFEntity* player );
        void AfterRunPlayer( KFEntity* player );

        // 创建角色
        void OnEnterCreatePlayer( KFEntity* player, uint64 playerid );

        // 启动上线同步数据定时器
        void StartSyncOnlineTimer( KFEntity* player );
        // 同步上线数据
        __KF_TIMER_FUNCTION__( OnTimerSyncEntityToOnline );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 添加属性
        __KF_COMMAND_FUNCTION__( OnCommandAddData );

        // 减少属性
        __KF_COMMAND_FUNCTION__( OnCommandDelData );

        // 设置属性
        __KF_COMMAND_FUNCTION__( OnCommandSetData );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新属性到客户端
        void SendUpdateDataToClient( KFEntity* player, KFMsg::PBObject& pbobect );

        // 同步添加属性到客户端
        void SendAddDataToClient( KFEntity* player, KFMsg::PBObject& pbobect );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( KFEntity* player, KFMsg::PBObject& pbobect );

        // 显示添加奖励消息
        void SendElementToClient( KFEntity* player, KFMsg::PBShowElements& pbelements );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 删除属性请求
        __KF_MESSAGE_FUNCTION__( HandleRemoveDataReq );

        // 请求同步属性
        __KF_MESSAGE_FUNCTION__( HandleRequestSyncReq );

        // 请求取消同步
        __KF_MESSAGE_FUNCTION__( HandleCancelSyncReq );

        // 请求更新数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateIntReq );
        __KF_MESSAGE_FUNCTION__( HandleUpdateStrReq );

        // 更新属性
        __KF_MESSAGE_FUNCTION__( HandleSyncUpdateDataFromServerReq );
        // 添加属性
        __KF_MESSAGE_FUNCTION__( HandleSyncAddDataFromServerReq );
        // 删除属性
        __KF_MESSAGE_FUNCTION__( HandleSyncRemoveDataFromServerReq );

        // 名字更新
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateNameCallBack );

    private:
        // 玩家组件
        KFComponent* _kf_component = nullptr;

        // 更新函数
        KFFunctionMap< std::string,  KFEntityFunction  > _player_run_function;
        KFFunctionMap< std::string, KFEntityFunction  > _player_after_run_function;

        // 初始化函数
        KFFunctionMap< std::string, KFEntityFunction > _player_init_function;

        // 卸载函数
        KFFunctionMap< std::string, KFEntityFunction > _player_uninit_function;

        // 重置函数
        KFFunctionMap< std::string, KFEntityFunction > _player_reset_function;

        // 登录函数
        KFFunctionMap< std::string,  KFEntityFunction > _player_before_enter_function;
        KFFunctionMap< std::string,  KFEntityFunction > _player_enter_function;
        KFFunctionMap< std::string,  KFEntityFunction > _player_after_enter_function;

        // 离开函数
        KFFunctionMap< std::string,  KFEntityFunction > _player_leave_function;

        // 新玩家处理函数
        KFFunctionMap< std::string,  KFEntityFunction > _new_player_function;

        // 创建角色函数
        KFFunctionMap< std::string, KFEntityFunction > _create_role_function;
    };
}

#endif