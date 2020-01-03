#include "KFMailClientModule.hpp"

namespace KFrame
{


    void KFMailClientModule::BeforeRun()
    {
        __REGISTER_ENTER_PLAYER__( &KFMailClientModule::OnEnterMailModule );
        __REGISTER_LEAVE_PLAYER__( &KFMailClientModule::OnLeaveMailModule );
        __REGISTER_NEW_PLAYER__( &KFMailClientModule::OnNewPlayerMailModule );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_MAIL_REQ, &KFMailClientModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_VIEW_MAIL_REQ, &KFMailClientModule::HandleViewMailReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DELETE_MAIL_REQ, &KFMailClientModule::HandleDeleteMailReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_MAIL_REWARD_REQ, &KFMailClientModule::HandleMailReceiveReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK, &KFMailClientModule::HandleQueryMailAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_ACK, &KFMailClientModule::HandleUpdateMailStatusAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &KFMailClientModule::HandleNoticeNewMailReq );
    }

    void KFMailClientModule::BeforeShut()
    {
        __UN_NEW_PLAYER__();
        __UN_ENTER_PLAYER__();
        __UN_LEAVE_PLAYER__();
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::MSG_QUERY_MAIL_REQ );
        __UN_MESSAGE__( KFMsg::MSG_VIEW_MAIL_REQ );
        __UN_MESSAGE__( KFMsg::MSG_DELETE_MAIL_REQ );
        __UN_MESSAGE__( KFMsg::MSG_MAIL_REWARD_REQ );

        __UN_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_ACK );
        __UN_MESSAGE__( KFMsg::S2S_NOTICE_NEW_MAIL_REQ );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMailClientModule::SendMessageToMail( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __STRING__( mail ), msgid, message, true );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ENTER_PLAYER_FUNCTION__( KFMailClientModule::OnEnterMailModule )
    {
        auto playerid = player->GetKeyID();
        __LOOP_TIMER_1__( playerid, KFTimeEnum::OneMinuteMicSecond * 5, 1, &KFMailClientModule::OnTimerQueryMail );
    }

    __KF_LEAVE_PLAYER_FUNCTION__( KFMailClientModule::OnLeaveMailModule )
    {
        auto playerid = player->GetKeyID();
        __UN_TIMER_1__( playerid );
    }

    __KF_NEW_PLAYER_FUNCTION__( KFMailClientModule::OnNewPlayerMailModule )
    {
        KFMsg::S2SNewPlayerMailReq req;
        req.set_playerid( player->GetKeyID() );
        req.set_zoneid( KFGlobal::Instance()->_app_id->GetZoneId() );
        SendMessageToMail( player->GetKeyID(), KFMsg::S2S_NEW_PLAYER_MAIL_REQ, &req );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_TIMER_FUNCTION__( KFMailClientModule::OnTimerQueryMail )
    {
        auto player = _kf_player->FindPlayer( objectid );
        if ( player == nullptr )
        {
            return __UN_TIMER_1__( objectid );
        }

        SendQueryMailMessage( player );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryMailReq );

        SendQueryMailMessage( player );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleNoticeNewMailReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SNoticeNewMailReq );

        SendQueryMailMessage( player );
    }

    void KFMailClientModule::SendQueryMailMessage( KFEntity* player )
    {
        auto maxmailid = player->Get( __STRING__( maxmailid ) );
        auto zoneid = KFGlobal::Instance()->_app_id->GetZoneId();

        KFMsg::S2SQueryMailReq req;
        req.set_zoneid( zoneid );
        req.set_maxid( maxmailid );
        req.set_playerid( player->GetKeyID() );
        SendMessageToMail( player->GetKeyID(), KFMsg::S2S_QUERY_MAIL_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryMailAck );

        // 将邮件保存到玩家属性中
        uint64 maxmailid = 0u;
        auto kfmailrecord = player->Find( __STRING__( mail ) );
        for ( auto i = 0; i < kfmsg.mail_size(); ++i )
        {
            auto pbmail = &kfmsg.mail( i );
            auto pbdata = &pbmail->data();

            // 初始化邮件内容
            auto kfmail = player->CreateData( kfmailrecord );
            for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
            {
                kfmail->Set( iter->first, iter->second );
            }
            player->AddData( kfmailrecord, kfmail );

            // 最大邮件id
            auto mailid = kfmail->GetKeyID();
            if ( mailid > maxmailid )
            {
                maxmailid = mailid;
            }
        }

        // 更新最大邮件id
        player->Set( __STRING__( maxmailid ), maxmailid );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMailClientModule::CheckMailTimeOut( KFData* kfmail )
    {
        auto validtime = kfmail->Get( __STRING__( validtime ) );
        auto sendtime = kfmail->Get( __STRING__( sendtime ) );

        return sendtime + validtime < KFGlobal::Instance()->_real_time;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleViewMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgViewMailReq );

        auto kfmail = player->Find( __STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        if ( CheckMailTimeOut( kfmail ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MailTimeOut );
        }

        auto status = kfmail->Get( __STRING__( status ) );
        if ( status != KFMsg::InitStatus )
        {
            return;
        }

        // 更新状态
        UpdateFlagToMail( player, kfmail, KFMsg::DoneStatus );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleDeleteMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDeleteMailReq );

        auto kfmail = player->Find( __STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        // 如果有奖励, 并且没有领取
        auto status = kfmail->Get( __STRING__( status ) );
        auto reward = kfmail->Get< std::string >( __STRING__( reward ) );
        if ( status != KFMsg::ReceiveStatus && !reward.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::MailDeleteFailed );
        }

        // 更新到邮件集群
        UpdateFlagToMail( player, kfmail, KFMsg::Remove );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleMailReceiveReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgMailRewardReq );

        auto kfmail = player->Find( __STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        auto status = kfmail->Get( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            return _kf_display->SendToClient( player, KFMsg::MailAlreadyReceived );
        }

        auto reward = kfmail->Get< std::string >( __STRING__( reward ) );
        if ( reward.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotHaveReward );
        }

        if ( CheckMailTimeOut( kfmail ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MailTimeOut );
        }

        UpdateFlagToMail( player, kfmail, KFMsg::ReceiveRemove );
    }

    void KFMailClientModule::UpdateFlagToMail( KFEntity* player, KFData* kfmail, uint32 status )
    {
        auto mailid = kfmail->GetKeyID();
        auto flag = kfmail->Get( __STRING__( flag ) );

        KFMsg::S2SUpdateMailStatusReq req;
        req.set_flag( flag );
        req.set_id( mailid );
        req.set_status( status );
        req.set_playerid( player->GetKeyID() );
        auto ok = SendMessageToMail( player->GetKeyID(), KFMsg::S2S_UPDATE_MAIL_STATUS_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::MailServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleUpdateMailStatusAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateMailStatusAck );

        switch ( kfmsg.status() )
        {
        case KFMsg::DoneStatus:
        {
            player->UpdateData( __STRING__( mail ), kfmsg.id(), __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
        }
        break;
        case KFMsg::Remove:
        {
            player->RemoveData( __STRING__( mail ), kfmsg.id() );
        }
        break;
        case KFMsg::ReceiveStatus:
        {
            ReceiveMailReward( player, kfmsg.id() );
        }
        break;
        case KFMsg::ReceiveRemove:
        {
            ReceiveMailReward( player, kfmsg.id() );
            player->RemoveData( __STRING__( mail ), kfmsg.id() );
        }
        break;
        default:
            break;
        }
    }

    void KFMailClientModule::ReceiveMailReward( KFEntity* player, uint64 id )
    {
        auto kfmail = player->Find( __STRING__( mail ), id );
        if ( kfmail == nullptr )
        {
            return __LOG_ERROR__( "player={} can't find mail={}", player->GetKeyID(), id );
        }

        auto reward = kfmail->Get< std::string >( __STRING__( reward ) );
        if ( reward.empty() )
        {
            return __LOG_ERROR__( "player={} mail={} no reward", player->GetKeyID(), id );
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( reward, __FUNC_LINE__ );
        if ( !ok )
        {
            return __LOG_ERROR__( "player={} mail={} reward={} error", player->GetKeyID(), id, reward );
        }

        player->UpdateData( kfmail, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );
        player->AddElement( &kfelements, __STRING__( mail ), __FUNC_LINE__ );
    }

    StringMap& KFMailClientModule::FormatMailData( KFEntity* sender, const KFMailSetting* kfsetting, const KFElements* kfelements )
    {
        // 有优化的空间, 服务器只发configid到客户端, 客户端根据邮件配置表来获得邮件的基础数据
        // 暂时先发给客户端, 省去客户端读取邮件配置表
        static StringMap _mail_data;
        _mail_data.clear();

        // 配置id
        _mail_data.insert( std::make_pair( __STRING__( configid ), __TO_STRING__( kfsetting->_id ) ) );

        // 类型
        _mail_data.insert( std::make_pair( __STRING__( type ), __TO_STRING__( kfsetting->_type ) ) );

        // 标题
        _mail_data.insert( std::make_pair( __STRING__( title ), kfsetting->_title ) );

        // 内容
        _mail_data.insert( std::make_pair( __STRING__( content ), kfsetting->_content ) );

        // 有效时间
        _mail_data.insert( std::make_pair( __STRING__( validtime ), __TO_STRING__( kfsetting->_valid_time ) ) );

        // 奖励
        if ( kfelements != nullptr )
        {
            _mail_data.insert( std::make_pair( __STRING__( reward ), kfelements->_str_element ) );
        }
        else if ( !kfsetting->_rewards._str_element.empty() )
        {
            _mail_data.insert( std::make_pair( __STRING__( reward ), kfsetting->_rewards._str_element ) );
        }

        // 发送者信息
        if ( sender != nullptr )
        {
            auto kfbasic = sender->Find( __STRING__( basic ) );

            // 发送者id
            _mail_data.insert( std::make_pair( __STRING__( sendid ), __TO_STRING__( sender->GetKeyID() ) ) );

            // 发送者名字
            _mail_data.insert( std::make_pair( __STRING__( sendname ), kfbasic->Get< std::string >( __STRING__( name ) ) ) );

            // 性别
            _mail_data.insert( std::make_pair( __STRING__( sex ), __TO_STRING__( kfbasic->Get< uint32 >( __STRING__( sex ) ) ) ) );
        }

        return _mail_data;
    }

    bool KFMailClientModule::SendAddMailToShard( uint64 sendid, uint32 flag, uint64 recvid, const StringMap& maildata )
    {
        KFMsg::S2SAddMailReq req;
        req.set_flag( flag );
        req.set_objectid( recvid );

        auto pbdata = req.mutable_pbmail()->mutable_data();
        for ( auto& iter : maildata )
        {
            ( *pbdata )[ iter.first ] = iter.second;
        }

        auto ok = SendMessageToMail( sendid, KFMsg::S2S_ADD_MAIL_REQ, &req );
        if ( !ok )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( req, &strdata );
            __LOG_ERROR__( "sendid={} mail={} failed", sendid, strdata );
        }

        return ok;
    }

    bool KFMailClientModule::SendMail( uint32 configid, const KFElements* kfelements )
    {
        auto kfsetting = KFMailConfig::Instance()->FindSetting( configid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto zoneid = KFGlobal::Instance()->_app_id->GetZoneId();

        auto& maildata = FormatMailData( nullptr, kfsetting, kfelements );
        return SendAddMailToShard( _invalid_int, KFMsg::GlobalMail, zoneid, maildata );
    }


    bool KFMailClientModule::SendMail( uint64 recvid, uint32 configid, const KFElements* kfelements )
    {
        auto kfsetting = KFMailConfig::Instance()->FindSetting( configid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( nullptr, kfsetting, kfelements );
        return SendAddMailToShard( _invalid_int, KFMsg::PersonMail, recvid, maildata );
    }

    bool KFMailClientModule::SendMail( KFEntity* player, uint64 recvid, uint32 configid, const KFElements* kfelements )
    {
        auto kfsetting = KFMailConfig::Instance()->FindSetting( configid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( player, kfsetting, kfelements );
        return SendAddMailToShard( player->GetKeyID(), KFMsg::PersonMail, recvid, maildata );
    }
}

