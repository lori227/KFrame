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
        if ( !CheckChatIntervalTime( kfentity ) )
        {
            return;
        }

        // 判断是否是好友
        auto kfrelation = kfentity->Find( __STRING__( friend ), kfmsg->playerid() );
        if ( kfrelation == nullptr )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ChatNotFriend );
        }
        auto kfbasic = kfrelation->Find( __STRING__( basic ) );

        // 判断屏蔽字符
        std::string content = kfmsg->content();
        _kf_filter->CensorFilter( content );

        KFMsg::MsgTellFriendChat chat;
        chat.set_content( content );
        chat.set_playerid( kfentity->GetKeyID() );
        auto ok = _kf_game->SendToPlayer( kfentity->GetKeyID(), kfbasic, KFMsg::MSG_TELL_FRIEND_CHAT, &chat );
        if ( !ok )
        {
            return _kf_display->SendToClient( kfentity, KFMsg::ChatFriendNotOnline );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFChatModule::HandleServerChatReq, KFMsg::MsgServerChatReq )
    {
        if ( !CheckChatIntervalTime( kfentity ) )
        {
            return;
        }

        // 判断屏蔽字符
        std::string content = kfmsg->content();
        _kf_filter->CensorFilter( content );

        auto kfbasic = kfentity->Find( __STRING__( basic ) );
        auto pbbasic = _kf_kernel->SerializeToView( kfbasic );

        KFMsg::MsgTellServerChat chat;
        chat.set_content( content );
        chat.mutable_player()->CopyFrom( *pbbasic );
        _kf_game->BroadcastToServer( KFMsg::MSG_TELL_SERVER_CHAT, &chat );
    }

    bool KFChatModule::CheckChatIntervalTime( KFEntity* player )
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