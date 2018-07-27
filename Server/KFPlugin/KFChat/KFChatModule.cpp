#include "KFChatModule.h"
#include "KFChatConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFChatModule::KFChatModule()
    {
    }

    KFChatModule::~KFChatModule()
    {

    }

    void KFChatModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_chat_config, false );
    }

    void KFChatModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFChatModule::OnEnterEnterChat );
        _kf_player->RegisterLeaveFunction( this, &KFChatModule::OnLeaveLeaveChat );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_ENTER_CHAT_REQ, &KFChatModule::HandleEnterChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_CHAT_REQ, &KFChatModule::HandleSendChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LEAVE_CHAT_REQ, &KFChatModule::HandleLeaveChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_ONEBYONE_CHAT_REQ, &KFChatModule::HandleSendOneByOneChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SEND_GROUPCHAT_REQ, &KFChatModule::HandleSendGroupChatReq );
    }

    void KFChatModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_ENTER_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LEAVE_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_ONEBYONE_CHAT_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_SEND_GROUPCHAT_REQ );
    }

    //处理加入江湖聊天室请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleEnterChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgEnterChatReq );

        //创建江湖聊天室玩家
        auto chattype = KFMsg::Jianghu;
        auto chatplayer =  _chat_player_list.Create( playerid );
        chatplayer->SetPlayerId( playerid );
        chatplayer->SetGateId( __KF_HEAD_ID__( kfguid ) );
        chatplayer->SetChatType( chattype );
    }

    //处理发送江湖聊天消息请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleSendChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgSendChatReq );

        auto chatplayer = _chat_player_list.Find( playerid );
        if ( chatplayer == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatNotFindPlayer );
        }

        auto chatsetting = _kf_chat_config->FindChatSetting( chatplayer->GetChatType() );
        if ( chatsetting == nullptr )
        {
            return;
        }

        //判断聊天内容字节数
        auto chatinfo = kfmsg.chatinfo();
        auto chatsize = chatinfo.size();
        if ( chatsize > chatsetting->_chat_byte_length )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatByteTooLong );
        }

        //判断发言时间间隔
        auto chatinterval = chatsetting->_chat_interval;
        auto nowtime = KFGlobal::Instance()->_real_time;
        if ( chatinterval + chatplayer->GetLastChatTime() > nowtime )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatIntervalError );
        }

        //设置发言时间
        chatplayer->SetLastChatTime( KFGlobal::Instance()->_real_time );

        //获取聊天数据和玩家数据
        auto kfobject = player->GetData();
        auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

        auto playername = kfbasic->GetValue< std::string >( __KF_STRING__( name ) );
        auto playericon = kfbasic->GetValue< std::string >( __KF_STRING__( icon ) );
        auto playersex = kfbasic->GetValue< uint32 >( __KF_STRING__( sex ) );
        auto playergrading = kfbasic->GetValue< uint32 >( __KF_STRING__( grading ) );
        auto playericonbox = kfbasic->GetValue< std::string >( __KF_STRING__( iconbox ) );
        auto playerserverid = kfbasic->GetValue< uint32 >( __KF_STRING__( serverid ) );

        KFMsg::MsgSendChatInfo info;
        info.set_chatinfo( kfmsg.chatinfo() );
        info.set_isvoice( kfmsg.isvoice() );
        info.set_playername( playername );
        info.set_playerid( playerid );
        info.set_playericon( playericon );
        info.set_playergrading( playergrading );
        info.set_playersex( playersex );
        info.set_playericonbox( playericonbox );
        info.set_playerserverid( playerserverid );
        for ( auto& iter : _chat_player_list._objects )
        {
            _kf_game->SendMessageToClient( iter.second->GetGateId(), iter.second->GetPlayerId(), KFMsg::MSG_SEND_CHAT_INFO, &info );
        }
    }

    //处理离开江湖聊天室请求
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleLeaveChatReq )
    {
        auto playerid = __KF_DATA_ID__( kfguid );
        _chat_player_list.Remove( playerid );
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

        auto playerid = static_cast< uint32 >( player->GetKeyID() );
        auto gateid = kfobject->GetValue< uint32 >( __KF_STRING__( gateid ) );

        //创建江湖聊天室玩家
        auto chatplayer = _chat_player_list.Create( playerid );
        chatplayer->SetPlayerId( playerid );
        chatplayer->SetGateId( gateid );
        chatplayer->SetChatType( KFMsg::Jianghu );
    }

    //离开回调
    void KFChatModule::OnLeaveLeaveChat( KFEntity* player )
    {
        auto kfobject = player->GetData();
        auto playerid = kfobject->GetValue( __KF_STRING__( basic ), __KF_STRING__( id ) );

        _chat_player_list.Remove( playerid );
    }
}
