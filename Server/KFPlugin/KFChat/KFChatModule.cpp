#include "KFChatModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFChatModule::InitModule()
    {
    }

    void KFChatModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFChatModule::OnEnterEnterChat );
        _kf_player->RegisterLeaveFunction( this, &KFChatModule::OnLeaveLeaveChat );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ENTER_CHAT_REQ, &KFChatModule::HandleEnterChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_CHAT_REQ, &KFChatModule::HandleSendChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LEAVE_CHAT_REQ, &KFChatModule::HandleLeaveChatReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SEND_CHAT_TO_SERVER, &KFChatModule::HandleSendChatToServerReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_ONEBYONE_CHAT_REQ, &KFChatModule::HandleSendOneByOneChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_GROUPCHAT_REQ, &KFChatModule::HandleSendGroupChatReq );
    }

    void KFChatModule::BeforeShut()
    {
        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ENTER_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LEAVE_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SEND_CHAT_TO_SERVER );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_ONEBYONE_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_GROUPCHAT_REQ );
    }

    //处理加入江湖聊天室请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleEnterChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgEnterChatReq );

        // 创建江湖聊天室玩家
        OnEnterEnterChat( player );
    }

    //处理发送江湖聊天消息请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleSendChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSendChatReq );

        auto chatinfo = kfmsg.chatinfo();

        auto chatplayer = _chat_player_list.Find( playerid );
        if ( chatplayer == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatNotFindPlayer );
        }

        //判断聊天内容字节数
        {
            static auto* kfoption = _kf_option->FindOption( __KF_STRING__( chatmaxlength ) );
            if ( chatinfo.length() > kfoption->_uint32_value )
            {
                return _kf_display->SendToClient( player, KFMsg::ChatByteTooLong );
            }
        }

        //判断发言时间间隔
        if ( KFGlobal::Instance()->_real_time < chatplayer->_last_chat_time )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatIntervalError );
        }

        //设置发言时间
        {
            static auto* kfoption = _kf_option->FindOption( __KF_STRING__( chatinterval ) );
            chatplayer->_last_chat_time = KFGlobal::Instance()->_real_time + kfoption->_uint32_value;
        }

        // _kf_filter->CensorFilter( chatinfo );

        //获取聊天数据和玩家数据
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        KFMsg::S2SSendChatToServer req;
        req.set_chatinfo( chatinfo );
        req.set_playerid( playerid );
        req.set_isvoice( kfmsg.isvoice() );
        req.set_playername( kfbasic->GetValue< std::string >( __KF_STRING__( name ) ) );
        req.set_playericon( kfbasic->GetValue< std::string >( __KF_STRING__( icon ) ) );
        req.set_playergrade( kfbasic->GetValue< uint32 >( __KF_STRING__( grade ) ) );
        req.set_playersex( kfbasic->GetValue< uint32 >( __KF_STRING__( sex ) ) );
        req.set_playericonbox( kfbasic->GetValue< std::string >( __KF_STRING__( iconbox ) ) );
        req.set_playerserverid( KFGlobal::Instance()->_app_id );
        _kf_game->TransmitToServer( KFMsg::S2S_SEND_CHAT_TO_SERVER, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleSendChatToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSendChatToServer );

        KFMsg::MsgSendChatInfo info;
        info.set_chatinfo( kfmsg.chatinfo() );
        info.set_isvoice( kfmsg.isvoice() );
        info.set_playername( kfmsg.playername() );
        info.set_playerid( kfmsg.playerid() );
        info.set_playericon( kfmsg.playericon() );
        info.set_playergrade( kfmsg.playergrade() );
        info.set_playersex( kfmsg.playersex() );
        info.set_playericonbox( kfmsg.playericonbox() );
        info.set_playerserverid( kfmsg.playerserverid() );
        for ( auto& iter : _chat_player_list._objects )
        {
            auto kfchatplayer = iter.second;
            _kf_game->SendToClient( kfchatplayer->_gate_id, kfchatplayer->_player_id, KFMsg::MSG_SEND_CHAT_INFO, &info );
        }
    }

    //处理离开江湖聊天室请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleLeaveChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgLeaveChatReq );

        OnLeaveLeaveChat( player );
    }

    //处理客户端发送一对一聊天请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleSendOneByOneChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSendOneByOneChatReq );

        if ( kfmsg.playerid() == _invalid_int || kfmsg.playerid() == playerid )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatFriendIdError );
        }

        auto kfobject = player->GetData();

        KFMsg::MsgSendOneByOneChatInfo info;
        info.set_chatinfo( kfmsg.chatinfo() );
        info.set_isvoice( kfmsg.isvoice() );
        info.set_playerid( playerid );
        info.set_chattype( kfmsg.chattype() );
        info.set_playerinfo( kfmsg.selfinfo() );
        info.set_serverid( kfobject->GetValue< uint32 >( __KF_STRING__( basic ), __KF_STRING__( serverid ) ) );

        _kf_route->SendMessageToRoute( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_SEND_ONEBYONE_CHAT_INFO, &info );
    }

    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleSendGroupChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSendGroupChatReq );

        KFMsg::MsgSendGroupChatInfo info;
        info.set_chatinfo( kfmsg.chatinfo() );
        info.set_isvoice( kfmsg.isvoice() );
        info.set_playerid( playerid );
        _kf_player->SendMessageToGroup( player, KFMsg::MSG_SEND_GROUPCHAT_INFO, &info );
    }

    void KFChatModule::OnEnterEnterChat( KFEntity* player )
    {
        auto kfobject = player->GetData();

        // 创建江湖聊天室玩家
        auto chatplayer = _chat_player_list.Create( player->GetKeyID() );
        chatplayer->_player_id = player->GetKeyID();
        chatplayer->_gate_id = kfobject->GetValue< uint32 >( __KF_STRING__( gateid ) );
    }

    //离开回调
    void KFChatModule::OnLeaveLeaveChat( KFEntity* player )
    {
        _chat_player_list.Remove( player->GetKeyID() );
    }
}
