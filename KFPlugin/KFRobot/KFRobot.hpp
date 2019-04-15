#ifndef __KF_ROBOT_H__
#define __KF_ROBOT_H__

#include "KFrame.h"

namespace KFrame
{
#define __KF_MESSAGE_FUNCTION__( function ) \
    void function( const Route& route, const char* data, uint32 length )

    class KFFsm;
    class KFData;
    class KFEntity;
    class KFComponent;
    class KFNetClientEngine;
    class KFRobot
    {
    public:
        KFRobot();
        ~KFRobot();

        // 初始化
        void Init( uint64 id, KFNetClientEngine* netclient );

        // 逻辑
        void Run();

        //////////////////////////////////////////////////////////////////////////
        // 发送消息
        bool SendNetMessage( uint32 msgid, google::protobuf::Message* message );

        // 改变状态
        void ChangeState( uint32 state );

        // 开始连接服务器
        void StartConnectGate();

        // 处理命令
        void ProcessCommand( const VectorString& params );

    public:
        // 同步添加对象
        void SyncAddObject( KFData* kfobject, const KFMsg::PBObject* pbobject );

        // 同步添加集合
        void SyncAddRecord( KFData* kfrecord, const KFMsg::PBRecord* pbrecord );

        // 同步更新属性
        void SyncUpdateData( KFData* kfobject, const KFMsg::PBObject* pbobject );

        // 删除属性
        void SyncRemoveData( KFData* kfobject, const KFMsg::PBObject* pbobject );

    public:
        // id
        uint64 _id = 0u;

        // 账号信息
        std::string _account;

        // gate ip port
        std::string _login_gate_ip;
        uint32 _login_gate_port = 0u;
        std::string _login_token;
        uint64 _account_id = 0u;

        // 服务器连接id
        uint64 _client_id = 0u;

        // 角色id
        uint64 _player_id = 0u;
        KFEntity* _player = nullptr;
        KFComponent* _kf_component = nullptr;

    private:
        // 客户端引擎
        KFNetClientEngine* _net_client;

        // 状态机
        KFFsm* _fsm;

        // 回调函数逻辑
        KFBind< std::string, const std::string&, KFCmdFunction > _cmd_function;
    };

}

#endif