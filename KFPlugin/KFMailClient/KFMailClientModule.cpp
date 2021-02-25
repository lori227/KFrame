#include "KFMailClientModule.hpp"

namespace KFrame
{


    void KFMailClientModule::BeforeRun()
    {
        __REGISTER_PLAYER_ENTER__( &KFMailClientModule::OnEnterMailModule );
        __REGISTER_PLAYER_LEAVE__( &KFMailClientModule::OnLeaveMailModule );
        __REGISTER_NEW_PLAYER__( &KFMailClientModule::OnNewPlayerMailModule );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_QUERY_MAIL_REQ, &KFMailClientModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_VIEW_MAIL_REQ, &KFMailClientModule::HandleViewMailReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_DELETE_MAIL_REQ, &KFMailClientModule::HandleDeleteMailReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::MSG_MAIL_REWARD_REQ, &KFMailClientModule::HandleMailReceiveReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_QUERY_MAIL_ACK, &KFMailClientModule::HandleQueryMailAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_UPDATE_MAIL_STATUS_ACK, &KFMailClientModule::HandleUpdateMailStatusAck );
        __REGISTER_MESSAGE__( KFMessageEnum::Player, KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &KFMailClientModule::HandleNoticeNewMailReq );
    }

    void KFMailClientModule::BeforeShut()
    {
        __UN_NEW_PLAYER__();
        __UN_PLAYER_ENTER__();
        __UN_PLAYER_LEAVE__();
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
    bool KFMailClientModule::SendMessageToMail( uint64 player_id, uint32 msg_id, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( player_id, __STRING__( mail ), msg_id, message );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_PLAYER_ENTER_FUNCTION__( KFMailClientModule::OnEnterMailModule )
    {
        auto player_id = player->GetKeyID();
        __LOOP_TIMER_1__( player_id, KFTimeEnum::OneMinuteMicSecond * 5, 1, &KFMailClientModule::OnTimerQueryMail );
    }

    __KF_PLAYER_LEAVE_FUNCTION__( KFMailClientModule::OnLeaveMailModule )
    {
        auto player_id = player->GetKeyID();
        __UN_TIMER_1__( player_id );
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

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailReq, KFMsg::MsgQueryMailReq )
    {
        SendQueryMailMessage( entity );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleNoticeNewMailReq, KFMsg::S2SNoticeNewMailReq )
    {
        SendQueryMailMessage( entity );
    }

    uint64 KFMailClientModule::GetMaxMailId( EntityPtr player )
    {
        auto max_mail_id = 0u;
        auto mail_record = player->Find( __STRING__( mail ) );
        for ( auto mail_data = mail_record->First(); mail_data != nullptr; mail_data = mail_record->Next() )
        {
            auto mail_id = mail_data->GetKeyID();
            if ( mail_id > max_mail_id )
            {
                max_mail_id = mail_id;
            }
        }

        return max_mail_id;
    }

    void KFMailClientModule::SendQueryMailMessage( EntityPtr player )
    {
        auto max_mail_id = GetMaxMailId( player );
        auto zone_id = KFGlobal::Instance()->_app_id->GetZoneId();

        KFMsg::S2SQueryMailReq req;
        req.set_zoneid( zone_id );
        req.set_maxid( max_mail_id );
        req.set_playerid( player->GetKeyID() );
        SendMessageToMail( player->GetKeyID(), KFMsg::S2S_QUERY_MAIL_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailAck, KFMsg::S2SQueryMailAck )
    {
        // 将邮件保存到玩家属性中
        auto mail_record = entity->Find( __STRING__( mail ) );
        for ( auto i = 0; i < kfmsg->mail_size(); ++i )
        {
            // 初始化邮件内容
            auto pb_data = &kfmsg->mail( i ).data();
            auto mail_data = entity->CreateData( mail_record );
            for ( auto iter = pb_data->begin(); iter != pb_data->end(); ++iter )
            {
                mail_data->Operate( iter->first, KFEnum::Set, iter->second );
            }
            entity->AddRecord( mail_record, mail_data );
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMailClientModule::CheckMailTimeOut( DataPtr mail_data )
    {
        auto valid_time = mail_data->Get( __STRING__( validtime ) );
        auto send_time = mail_data->Get( __STRING__( sendtime ) );

        return send_time + valid_time < KFGlobal::Instance()->_real_time;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleViewMailReq, KFMsg::MsgViewMailReq )
    {
        auto mail_data = entity->Find( __STRING__( mail ), kfmsg->id() );
        if ( mail_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailNotExist );
        }

        if ( CheckMailTimeOut( mail_data ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailTimeOut );
        }

        auto status = mail_data->Get( __STRING__( status ) );
        if ( status != KFMsg::InitStatus )
        {
            return;
        }

        // 更新状态
        UpdateMailStatusToShard( entity, mail_data, KFMsg::DoneStatus );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleDeleteMailReq, KFMsg::MsgDeleteMailReq )
    {
        auto mail_data = entity->Find( __STRING__( mail ), kfmsg->id() );
        if ( mail_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailNotExist );
        }

        // 如果有奖励, 并且没有领取
        auto status = mail_data->Get( __STRING__( status ) );
        auto reward = mail_data->Get<std::string>( __STRING__( reward ) );
        if ( status != KFMsg::ReceiveStatus && !reward.empty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailDeleteFailed );
        }

        // 更新到邮件集群
        UpdateMailStatusToShard( entity, mail_data, KFMsg::Remove );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleMailReceiveReq, KFMsg::MsgMailRewardReq )
    {
        auto mail_data = entity->Find( __STRING__( mail ), kfmsg->id() );
        if ( mail_data == nullptr )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailNotExist );
        }

        auto status = mail_data->Get( __STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailAlreadyReceived );
        }

        auto reward = mail_data->Get<std::string>( __STRING__( reward ) );
        if ( reward.empty() )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailNotHaveReward );
        }

        if ( CheckMailTimeOut( mail_data ) )
        {
            return _kf_display->SendToClient( entity, KFMsg::MailTimeOut );
        }

        UpdateMailStatusToShard( entity, mail_data, KFMsg::ReceiveRemove );
    }

    void KFMailClientModule::UpdateMailStatusToShard( EntityPtr player, DataPtr mail_data, uint32 status )
    {
        auto mail_id = mail_data->GetKeyID();
        auto flag = mail_data->Get( __STRING__( flag ) );

        KFMsg::S2SUpdateMailStatusReq req;
        req.set_flag( flag );
        req.set_id( mail_id );
        req.set_status( status );
        req.set_playerid( player->GetKeyID() );
        auto ok = SendMessageToMail( player->GetKeyID(), KFMsg::S2S_UPDATE_MAIL_STATUS_REQ, &req );
        if ( !ok )
        {
            _kf_display->SendToClient( player, KFMsg::MailServerBusy );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleUpdateMailStatusAck, KFMsg::S2SUpdateMailStatusAck )
    {
        switch ( kfmsg->status() )
        {
        case KFMsg::DoneStatus:
        {
            entity->UpdateRecord( __STRING__( mail ), kfmsg->id(), __STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
        }
        break;
        case KFMsg::Remove:
        {
            entity->RemoveRecord( __STRING__( mail ), kfmsg->id() );
        }
        break;
        case KFMsg::ReceiveStatus:
        {
            ReceiveMailReward( entity, kfmsg->id() );
        }
        break;
        case KFMsg::ReceiveRemove:
        {
            ReceiveMailReward( entity, kfmsg->id() );
            entity->RemoveRecord( __STRING__( mail ), kfmsg->id() );
        }
        break;
        default:
            break;
        }
    }

    void KFMailClientModule::ReceiveMailReward( EntityPtr player, uint64 id )
    {
        auto mail_data = player->Find( __STRING__( mail ), id );
        if ( mail_data == nullptr )
        {
            return __LOG_ERROR__( "player={} can't find mail={}", player->GetKeyID(), id );
        }

        auto reward = mail_data->Get<std::string>( __STRING__( reward ) );
        if ( reward.empty() )
        {
            return __LOG_ERROR__( "player={} mail={} no reward", player->GetKeyID(), id );
        }

        KFElements elements;
        elements._str_parse = reward;
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( !ok )
        {
            return __LOG_ERROR__( "player={} mail={} reward={} error", player->GetKeyID(), id, reward );
        }

        player->UpdateObject( mail_data, __STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );
        player->AddElement( &elements, _default_multiple, __STRING__( mail ), id, __FUNC_LINE__ );
    }

    StringMap& KFMailClientModule::FormatMailData( EntityPtr sender, std::shared_ptr<const KFMailSetting> setting, const KFElements* elements )
    {
        // 有优化的空间, 服务器只发configid到客户端, 客户端根据邮件配置表来获得邮件的基础数据
        // 暂时先发给客户端, 省去客户端读取邮件配置表
        static StringMap _mail_data;
        _mail_data.clear();

        // 配置id
        _mail_data.insert( std::make_pair( __STRING__( configid ), __TO_STRING__( setting->_id ) ) );

        // 类型
        _mail_data.insert( std::make_pair( __STRING__( type ), __TO_STRING__( setting->_type ) ) );

        // 标题
        _mail_data.insert( std::make_pair( __STRING__( title ), setting->_title ) );

        // 内容
        _mail_data.insert( std::make_pair( __STRING__( content ), setting->_content ) );

        // 有效时间
        _mail_data.insert( std::make_pair( __STRING__( validtime ), __TO_STRING__( setting->_valid_time ) ) );

        // 奖励
        if ( elements != nullptr )
        {
            _mail_data.insert( std::make_pair( __STRING__( reward ), elements->GetElement() ) );
        }
        else if ( !setting->_reward.GetElement().empty() )
        {
            _mail_data.insert( std::make_pair( __STRING__( reward ), setting->_reward.GetElement() ) );
        }

        // 发送者信息
        if ( sender != nullptr )
        {
            auto basic_data = sender->Find( __STRING__( basic ) );

            // 发送者id
            _mail_data.insert( std::make_pair( __STRING__( sendid ), __TO_STRING__( sender->GetKeyID() ) ) );

            // 发送者名字
            _mail_data.insert( std::make_pair( __STRING__( sendname ), basic_data->Get<std::string>( __STRING__( name ) ) ) );

            // 性别
            _mail_data.insert( std::make_pair( __STRING__( sex ), __TO_STRING__( basic_data->Get<uint32>( __STRING__( sex ) ) ) ) );
        }

        return _mail_data;
    }

    bool KFMailClientModule::SendAddMailToShard( uint64 send_id, uint32 flag, uint64 recv_id, const StringMap& mail_data )
    {
        KFMsg::S2SAddMailReq req;
        req.set_flag( flag );
        req.set_objectid( recv_id );

        auto& pb_data = *req.mutable_pbmail()->mutable_data();
        __MAP_TO_PROTO__( mail_data, pb_data );
        auto ok = SendMessageToMail( sendid, KFMsg::S2S_ADD_MAIL_REQ, &req );
        if ( !ok )
        {
            std::string str_data;
            google::protobuf::util::MessageToJsonString( req, &str_data );
            __LOG_ERROR__( "sendid={} mail={} failed", sendid, str_data );
        }

        return ok;
    }

    bool KFMailClientModule::SendMail( uint32 config_id, const KFElements* elements )
    {
        auto setting = KFMailConfig::Instance()->FindSetting( config_id );
        if ( setting == nullptr )
        {
            return false;
        }

        auto zone_id = KFGlobal::Instance()->_app_id->GetZoneId();

        auto& mail_data = FormatMailData( nullptr, setting, elements );
        return SendAddMailToShard( _invalid_int, KFMsg::GlobalMail, zone_id, mail_data );
    }


    bool KFMailClientModule::SendMail( uint64 recv_id, uint32 config_id, const KFElements* elements )
    {
        auto setting = KFMailConfig::Instance()->FindSetting( config_id );
        if ( setting == nullptr )
        {
            return false;
        }

        auto& mail_data = FormatMailData( nullptr, setting, elements );
        return SendAddMailToShard( _invalid_int, KFMsg::PersonMail, recv_id, mail_data );
    }

    bool KFMailClientModule::SendMail( EntityPtr player, uint64 recv_id, uint32 config_id, const KFElements* elements )
    {
        auto setting = KFMailConfig::Instance()->FindSetting( configid );
        if ( setting == nullptr )
        {
            return false;
        }

        auto& mail_data = FormatMailData( player, setting, elements );
        return SendAddMailToShard( player->GetKeyID(), KFMsg::PersonMail, recv_id, mail_data );
    }
}

