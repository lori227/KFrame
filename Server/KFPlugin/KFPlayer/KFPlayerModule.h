#ifndef __KF_PLAYER_MODULE_H__
#define __KF_PLAYER_MODULE_H__

/************************************************************************
//    @Moudle			:    游戏玩家基础功能模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-20
************************************************************************/

#include "KFrame.h"
#include "KFPlayerInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFMessage/KFMessageInterface.h"
#include "KFGame/KFGameInterface.h"
#include "KFZone/KFZoneInterface.h"
#include "KFDisplay/KFDisplayInterface.h"
#include "KFTcpClient/KFTcpClientInterface.h"
#include "KFDataClient/KFDataClientInterface.h"
#include "KFRouteClient/KFRouteClientInterface.h"
#include "KFPublicClient/KFPublicClientInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"
#include "KFDebug/KFDebugInterface.h"

namespace KFrame
{
    class KFComponent;
    class KFPlayerModule : public KFPlayerInterface
    {
    public:
        KFPlayerModule();
        ~KFPlayerModule();

        // 初始化
        virtual void InitModule();

        // 刷新
        virtual void BeforeRun();

        // 关闭
        virtual void BeforeShut();
        ////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////

        // 初始化
        void InitPlayer( KFEntity* kfentity );
        void UnInitPlayer( KFEntity* kfentity );

        // 逻辑刷新
        void RunPlayer( KFEntity* kfentity );

        // 保存玩家
        void SavePlayer( KFEntity* kfentity );
        ////////////////////////////////////////////////////////////////////////////
        // 玩家数量
        virtual uint32 GetPlayerCount();

        // 创建角色
        virtual KFEntity* CreatePlayer( uint32 gateid, uint32 playerid, const KFMsg::PBObject* playerdata, const KFMsg::PBStrings* extenddata );

        // 登出游戏
        virtual void LogoutPlayer( uint32 playerid );

        // 踢掉角色
        virtual void KickPlayer( uint32 playerid, uint32 type, const char* function, uint32 line );

        // 查找玩家
        virtual KFEntity* FindPlayer( uint32 playerid, const char* function, uint32 line );

        // 发送消息
        virtual bool SendMessageToClient( uint32 playerid, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToClient( uint32 playerid, uint32 msgid, const char* data, uint32 length );

        virtual bool SendMessageToClient( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message );
        virtual bool SendMessageToClient( KFEntity* player, uint32 msgid, const char* data, uint32 length );

        virtual bool SendMessageToClient( KFData* kfbasic, uint32 msgid, ::google::protobuf::Message* message );

        virtual void SendMessageToGroup( KFEntity* player, uint32 msgid, ::google::protobuf::Message* message, bool sendself = true );

    protected:
        virtual void AddInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveInitDataFunction( const std::string& moudle );

        virtual void AddUnInitDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveUnInitDataFunction( const std::string& moudle );

        virtual void AddRunDataFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveRunDataFunction( const std::string& moudle );

        virtual void AddEnterFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveEnterFunction( const std::string& moudle );

        virtual void AddLeaveFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLeaveFunction( const std::string& moudle );

        virtual void AddLogoutFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveLogoutFunction( const std::string& moudle );

        virtual void AddNewPlayerFunction( const std::string& moudle, KFEntityFunction& function );
        virtual void RemoveNewPlayerFunction( const std::string& moudle );

    protected:
        // 连接World服务器
        __KF_CLIENT_CONNECT_FUNCTION__( OnClientConnectionWorld );

        // 转发消息到玩家
        __KF_TRANSMIT_FUNCTION__( TransmitMessageToPlayer );

        // 部署服务器关闭
        __KF_COMMAND_FUNCTION__( OnDeployShutDownServer );

        // 添加属性
        __KF_DEBUG_FUNCTION__( DebugAddData );

        // 减少属性
        __KF_DEBUG_FUNCTION__( DebugDecData );

        // 设置属性
        __KF_DEBUG_FUNCTION__( DebugSetData );

    protected:
        // 处理玩家掉线
        __KF_MESSAGE_FUNCTION__( HandlePlayerDisconnectionReq );

        // 处理踢人消息
        __KF_MESSAGE_FUNCTION__( HandleKickGamePlayerReq );

        // 处理创建角色
        __KF_MESSAGE_FUNCTION__( HandleCreateRoleReq );

        // 处理创建角色
        __KF_MESSAGE_FUNCTION__( HandleCreateRoleAck );

        // 处理修改名字请求
        __KF_MESSAGE_FUNCTION__( HandleChangeNameReq );

        // 处理设置性别请求
        __KF_MESSAGE_FUNCTION__( HandleChangeSexReq );

        // 处理设置名字回馈
        __KF_MESSAGE_FUNCTION__( HandleSetPlayerNameAck );

        // 请求修改人物头像
        __KF_MESSAGE_FUNCTION__( HandleChangeIconReq );

        // 处理修改头像框请求
        __KF_MESSAGE_FUNCTION__( HandleChangeIconBoxReq );

        // 请求修改个性签名
        __KF_MESSAGE_FUNCTION__( HandleChangeMottoReq );

        // 查询玩家所有数据
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerReq );

        // 处理查询玩家所有数据回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryPlayerAck );

        // 删除属性请求
        __KF_MESSAGE_FUNCTION__( HandleRemoveDataReq );

        // 查询访客信息
        __KF_MESSAGE_FUNCTION__( HandleQueryGuestReq );

        // 查询访客信息回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryGuestAck );

        // 查询总的被敬酒次数
        __KF_MESSAGE_FUNCTION__( HandleQueryToastCountReq );

        // 查询总的被敬酒次数回馈
        __KF_MESSAGE_FUNCTION__( HandleQueryToastCountAck );

        // 查询玩家设置请求
        __KF_MESSAGE_FUNCTION__( HandleQuerySettingReq );

        // 玩家更新设置请求
        __KF_MESSAGE_FUNCTION__( HandleUpdateSettingReq );

    private:
        // 创建角色
        void OnEnterCreatePlayer( KFEntity* player, uint32 gateid, uint32 playerid );

        // 进入游戏世界
        void SendEnterMessageToWorld( KFEntity* player );

        // 离开游戏世界
        void SendLeaveMessageToWord( KFEntity* player );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 同步更新属性到客户端
        void SendUpdateDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步添加属性到客户端
        void SendAddDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 同步删除属性到客户端
        void SendRemoveDataToClient( KFEntity* player, const KFMsg::PBObject& pbobect );

        // 显示添加奖励消息
        void SendRewardAgentToClient( KFEntity* player, const std::string& reward, bool showclient, const char* function, uint32 line );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    private:
        // 玩家组件
        KFComponent* _kf_component;

        // 玩家数据
        KFData* _kf_player_data;

        // 更新函数
        KFBind< std::string, const std::string&, KFEntityFunction  > _player_run_function;

        // 初始化函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_init_function;

        // 卸载函数
        KFBind< std::string, const std::string&, KFEntityFunction > _player_uninit_function;

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