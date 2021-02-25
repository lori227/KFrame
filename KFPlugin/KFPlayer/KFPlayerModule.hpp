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
        virtual EntityPtr Login( const KFMsg::PBLoginData* login_data, const KFMsg::PBObject* player_data );
        virtual EntityPtr ReLogin( uint64 player_id, uint64 gate_id );

        // 删除玩家
        virtual void Logout();
        virtual void Logout( uint64 player_id );
        virtual void Logout( EntityPtr player );
        ////////////////////////////////////////////////////////////////////////////////

        // 查找玩家
        virtual EntityPtr FindPlayer( uint64 player_id );

        virtual bool SendToClient( EntityPtr player, uint32 msg_id, ::google::protobuf::Message* message, uint32 delay = 0 );
        ////////////////////////////////////////////////////////////////////////////////
    protected:
        virtual void AddRunDataFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveRunDataFunction( KFModule* module );

        virtual void AddAfterRunDataFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveAfterRunDataFunction( KFModule* module );

        virtual void AddResetFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveResetFunction( KFModule* module );

        virtual void AddBeforeEnterFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveBeforeEnterFunction( KFModule* module );

        virtual void AddEnterFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveEnterFunction( KFModule* module );

        virtual void AddAfterEnterFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveAfterEnterFunction( KFModule* module );

        virtual void AddLeaveFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveLeaveFunction( KFModule* module );

        virtual void AddNewPlayerFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveNewPlayerFunction( KFModule* module );

        virtual void AddCreateRoleFunction( KFModule* module, KFEntityFunction& function );
        virtual void RemoveCreateRoleFunction( KFModule* module );

    protected:
        // 逻辑刷新
        void RunPlayer( EntityPtr player );
        void AfterRunPlayer( EntityPtr player );

        // 创建角色
        void OnEnterCreatePlayer( EntityPtr player, uint64 player_id );

        // 启动上线同步数据定时器
        void StartSyncOnlineTimer( EntityPtr player );
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
        void SendUpdateDataToClient( EntityPtr player, KFMsg::PBObject& message );

        // 同步添加属性到客户端
        void SendAddDataToClient( EntityPtr player, KFMsg::PBObject& message );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( EntityPtr player, KFMsg::PBObject& message );

        // 显示添加奖励消息
        void SendElementToClient( EntityPtr player, KFMsg::PBShowElements& show_elements );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 删除属性请求
        __KF_MESSAGE_FUNCTION__( HandleRemoveDataReq, KFMsg::MsgRemoveDataReq );

        // 请求同步属性
        __KF_MESSAGE_FUNCTION__( HandleRequestSyncReq, KFMsg::MsgRequestSyncReq );

        // 请求取消同步
        __KF_MESSAGE_FUNCTION__( HandleCancelSyncReq, KFMsg::MsgCancelSyncReq );

        // 请求更新数据
        __KF_MESSAGE_FUNCTION__( HandleUpdateIntReq, KFMsg::MsgUpdateIntReq );
        __KF_MESSAGE_FUNCTION__( HandleUpdateStrReq, KFMsg::MsgUpdateStrReq );

        // 更新属性
        __KF_MESSAGE_FUNCTION__( HandleSyncUpdateDataFromServerReq, KFMsg::S2SSyncUpdateDataFromServer );
        // 添加属性
        __KF_MESSAGE_FUNCTION__( HandleSyncAddDataFromServerReq, KFMsg::S2SSyncAddDataFromServer );
        // 删除属性
        __KF_MESSAGE_FUNCTION__( HandleSyncRemoveDataFromServerReq, KFMsg::S2SSyncRemoveDataFromServer );

        // 名字更新
        __KF_UPDATE_STRING_FUNCTION__( OnUpdateNameCallBack );

    private:
        // 玩家组件
        std::shared_ptr<KFComponent> _component = nullptr;

        // 更新函数
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _player_run_function;
        KFMapModuleFunction<KFModule*, KFEntityFunction> _player_after_run_function;

        // 重置函数
        KFMapModuleFunction<KFModule*, KFEntityFunction> _player_reset_function;

        // 登录函数
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _player_before_enter_function;
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _player_enter_function;
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _player_after_enter_function;

        // 离开函数
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _player_leave_function;

        // 新玩家处理函数
        KFMapModuleFunction<KFModule*,  KFEntityFunction> _new_player_function;

        // 创建角色函数
        KFMapModuleFunction<KFModule*, KFEntityFunction> _create_role_function;
    };
}

#endif