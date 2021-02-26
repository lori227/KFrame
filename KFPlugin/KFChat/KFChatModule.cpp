#include "KFChatModule.hpp"

namespace KFrame
{
    void KFChatModule::BeforeRun()
    {
        __REGISTER_PLAYER_LEAVE__( &KFChatModule::OnLeaveChatModule );
        ///////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_FRIEND_CHAT_REQ, &KFChatModule::HandleFriendChatReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_SERVER_CHAT_REQ, &KFChatModule::HandleServerChatReq );
    }

    void KFChatModule::BeforeShut()
    {
        __UN_PLAYER_LEAVE__();
        ///////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_FRIEND_CHAT_REQ );
        __UN_MESSAGE__( KFMsg::MSG_SERVER_CHAT_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleFriendChatReq, KFMsg::MsgFriendChatReq )
    {
        if ( !CheckChatIntervalTime( player ) )
        {
            return;
        }

        // 判断是否是好友
        auto relation_data = player->Find( __STRING__( friend ), kfmsg->playerid() );
        if ( relation_data == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatNotFriend );
        }
        auto basic_data = relation_data->Find( __STRING__( basic ) );

        // 判断屏蔽字符
        std::string content = kfmsg->content();
        _kf_filter->CensorFilter( content );

        KFMsg::MsgTellFriendChat chat;
        chat.set_content( content );
        chat.set_playerid( player->GetKeyID() );
        auto ok = _kf_game->SendToPlayer( player->GetKeyID(), basic_data, KFMsg::MSG_TELL_FRIEND_CHAT, &chat );
        if ( !ok )
        {
            return _kf_display->SendToClient( player, KFMsg::ChatFriendNotOnline );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleServerChatReq, KFMsg::MsgServerChatReq )
    {
        if ( !CheckChatIntervalTime( player ) )
        {
            return;
        }

        // 判断屏蔽字符
        std::string content = kfmsg->content();
        _kf_filter->CensorFilter( content );

        auto basic_data = player->Find( __STRING__( basic ) );
        auto pb_basic = _kf_kernel->SerializeToView( basic_data );

        KFMsg::MsgTellServerChat chat;
        chat.set_content( content );
        chat.mutable_player()->CopyFrom( *pb_basic );
        _kf_game->BroadcastToServer( KFMsg::MSG_TELL_SERVER_CHAT, &chat );
    }

    bool KFChatModule::CheckChatIntervalTime( EntityPtr player )
    {
        auto iter = _chat_interval_time.find( player->GetKeyID() );
        if ( iter != _chat_interval_time.end() )
        {
            if ( KFGlobal::Instance()->_game_time < iter->second )
            {
                _kf_display->SendToClient( player, KFMsg::ChatIntervalTimeLimit );
                return false;
            }
        }

        static auto _chat_constant = KFGlobal::Instance()->FindConstant( "chatintervaltime" );
        _chat_interval_time[ player->GetKeyID() ] = KFGlobal::Instance()->_game_time + _chat_constant->_uint32_value;
        return true;
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFChatModule::OnLeaveChatModule )
    {
        _chat_interval_time.erase( player->GetKeyID() );
    }
}