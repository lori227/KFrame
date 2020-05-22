#include "KFChatModule.hpp"

namespace KFrame
{
    void KFChatModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_FRIEND_CHAT_REQ, &KFChatModule::HandleFriendChatReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_SERVER_CHAT_REQ, &KFChatModule::HandleServerChatReq );
    }

    void KFChatModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_FRIEND_CHAT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SERVER_CHAT_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleFriendChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgFriendChatReq );

        // 判断是否是好友
        auto kfrelation = player->Find( __STRING__( friend ), kfmsg.playerid() );
        if ( kfrelation == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatNotFriend );
        }
        auto kfbasic = kfrelation->Find( __STRING__( basic ) );

        // 判断屏蔽字符
        std::string content = kfmsg.content();
        _kf_filter->CensorFilter( content );

        KFMsg::MsgTellFriendChat chat;
        chat.set_playerid( playerid );
        chat.set_content( content );
        auto ok = _kf_game->SendToPlayer( playerid, kfbasic, KFMsg::MSG_TELL_FRIEND_CHAT, &chat );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatFriendNotOnline );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleServerChatReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgServerChatReq );

        // 判断屏蔽字符
        std::string content = kfmsg.content();
        _kf_filter->CensorFilter( content );

        auto kfbasic = player->Find( __STRING__( basic ) );
        auto pbbasic = _kf_kernel->SerializeToView( kfbasic );

        KFMsg::MsgTellServerChat chat;
        chat.set_content( content );
        chat.mutable_player()->CopyFrom( *pbbasic );
        _kf_game->BroadcastToServer( KFMsg::MSG_TELL_SERVER_CHAT, &chat );
    }
}