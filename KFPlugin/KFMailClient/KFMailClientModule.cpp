#include "KFMailClientModule.hpp"
#include "KFMailConfig.hpp"

namespace KFrame
{
    void KFMailClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_mail_config, true );
    }

    void KFMailClientModule::BeforeRun()
    {
        _kf_player->RegisterEnterFunction( this, &KFMailClientModule::OnEnterQueryMail );
        _kf_player->RegisterLeaveFunction( this, &KFMailClientModule::OnLeaveQueryMail );
        _kf_player->RegisterNewPlayerFunction( this, &KFMailClientModule::OnNewPlayerMail );
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
        __KF_REMOVE_CONFIG__( _kf_mail_config );

        _kf_player->UnRegisterEnterFunction( this );
        _kf_player->UnRegisterLeaveFunction( this );
        _kf_player->UnRegisterNewPlayerFunction( this );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_QUERY_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_VIEW_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DELETE_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_MAIL_REWARD_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NOTICE_NEW_MAIL_REQ );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFMailClientModule::SendMessageToMail( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_route->SendToRand( playerid, __KF_STRING__( mail ), msgid, message );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMailClientModule::OnEnterQueryMail( KFEntity* player )
    {
        auto playerid = player->GetKeyID();
        __REGISTER_LOOP_TIMER__( playerid, KFTimeEnum::OneMinuteMicSecond * 5, 0, &KFMailClientModule::OnTimerQueryMail );
    }

    void KFMailClientModule::OnLeaveQueryMail( KFEntity* player )
    {
        auto playerid = player->GetKeyID();
        __UNREGISTER_OBJECT_TIMER__( playerid );
    }

    void KFMailClientModule::OnNewPlayerMail( KFEntity* player )
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
            return __UNREGISTER_OBJECT_TIMER__( objectid );
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
        auto kfobject = player->GetData();
        auto maxmailid = kfobject->GetValue( __KF_STRING__( maxmailid ) );

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

        auto kfobject = player->GetData();
        auto kfmailrecord = kfobject->FindData( __KF_STRING__( mail ) );

        uint64 maxmailid = 0u;

        // 将邮件保存到玩家属性中
        for ( auto i = 0; i < kfmsg.mail_size(); ++i )
        {
            auto pbmail = &kfmsg.mail( i );
            auto kfmail = ParsePBMailToData( pbmail, kfmailrecord->GetDataSetting() );
            if ( kfmail == nullptr )
            {
                continue;
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
        kfobject->SetValue( __KF_STRING__( maxmailid ), maxmailid );
    }

    KFData* KFMailClientModule::ParsePBMailToData( const KFMsg::PBMail* pbmail, const KFDataSetting* kfsetting )
    {
        auto kfmail = _kf_kernel->CreateObject( kfsetting );
        if ( kfmail == nullptr )
        {
            return nullptr;
        }

        // 设置邮件属性
        auto pbdata = &pbmail->data();
        for ( auto iter = pbdata->begin(); iter != pbdata->end(); ++iter )
        {
            kfmail->SetValue( iter->first, iter->second );
        }

        return kfmail;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool KFMailClientModule::CheckMailTimeOut( KFData* kfmail )
    {
        auto validtime = kfmail->GetValue( __KF_STRING__( validtime ) );
        auto sendtime = kfmail->GetValue( __KF_STRING__( sendtime ) );

        return sendtime + validtime < KFGlobal::Instance()->_real_time;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleViewMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgViewMailReq );

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        if ( CheckMailTimeOut( kfmail ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MailTimeOut );
        }

        auto status = kfmail->GetValue( __KF_STRING__( status ) );
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

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        // 如果有奖励, 并且没有领取
        auto status = kfmail->GetValue( __KF_STRING__( status ) );
        auto reward = kfmail->GetValue< std::string >( __KF_STRING__( reward ) );
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

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.id() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        auto status = kfmail->GetValue( __KF_STRING__( status ) );
        if ( status == KFMsg::ReceiveStatus )
        {
            return _kf_display->SendToClient( player, KFMsg::MailAlreadyReceived );
        }

        auto reward = kfmail->GetValue< std::string >( __KF_STRING__( reward ) );
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
        auto flag = kfmail->GetValue( __KF_STRING__( flag ) );

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
            player->UpdateData( __KF_STRING__( mail ), kfmsg.id(), __KF_STRING__( status ), KFEnum::Set, KFMsg::DoneStatus );
        }
        break;
        case KFMsg::Remove:
        {
            player->RemoveData( __KF_STRING__( mail ), kfmsg.id() );
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
            player->RemoveData( __KF_STRING__( mail ), kfmsg.id() );
        }
        break;
        default:
            break;
        }
    }

    void KFMailClientModule::ReceiveMailReward( KFEntity* player, uint64 id )
    {
        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), id );
        if ( kfmail == nullptr )
        {
            return __LOG_ERROR__( "player={} can't find mail={}!", player->GetKeyID(), id );
        }

        auto reward = kfmail->GetValue< std::string >( __KF_STRING__( reward ) );
        if ( reward.empty() )
        {
            return __LOG_ERROR__( "player={} mail={} no reward!", player->GetKeyID(), id );
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( reward, __FUNC_LINE__ );
        if ( !ok )
        {
            return __LOG_ERROR__( "player={} mail={} reward={} error!", player->GetKeyID(), id, reward );
        }

        player->UpdateData( kfmail, __KF_STRING__( status ), KFEnum::Set, KFMsg::ReceiveStatus );
        player->AddElement( &kfelements, true, __FUNC_LINE__ );
    }

    MapString& KFMailClientModule::FormatMailData( KFEntity* sender, const KFMailSetting* kfsetting, const KFElements* kfelements )
    {
        // 有优化的空间, 服务器只发configid到客户端, 客户端根据邮件配置表来获得邮件的基础数据
        // 暂时先发给客户端, 省去客户端读取邮件配置表
        static MapString _mail_data;
        _mail_data.clear();

        // 配置id
        _mail_data.insert( std::make_pair( __KF_STRING__( configid ), __TO_STRING__( kfsetting->_config_id ) ) );

        // 类型
        _mail_data.insert( std::make_pair( __KF_STRING__( type ), __TO_STRING__( kfsetting->_type ) ) );

        // 标题
        _mail_data.insert( std::make_pair( __KF_STRING__( title ), kfsetting->_title ) );

        // 内容
        _mail_data.insert( std::make_pair( __KF_STRING__( content ), kfsetting->_content ) );

        // 有效时间
        _mail_data.insert( std::make_pair( __KF_STRING__( validtime ), __TO_STRING__( kfsetting->_valid_time ) ) );

        // 奖励
        if ( kfelements != nullptr )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( reward ), kfelements->_str_element ) );
        }
        else if ( !kfsetting->_rewards._str_element.empty() )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( reward ), kfsetting->_rewards._str_element ) );
        }

        // 发送者信息
        if ( sender != nullptr )
        {
            auto kfobject = sender->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

            // 发送者id
            _mail_data.insert( std::make_pair( __KF_STRING__( sendid ), __TO_STRING__( kfobject->GetKeyID() ) ) );

            // 发送者名字
            _mail_data.insert( std::make_pair( __KF_STRING__( sendname ), kfbasic->GetValue< std::string >( __KF_STRING__( name ) ) ) );

            // 性别
            _mail_data.insert( std::make_pair( __KF_STRING__( sex ), __TO_STRING__( kfbasic->GetValue< uint32 >( __KF_STRING__( sex ) ) ) ) );
        }

        return _mail_data;
    }

    bool KFMailClientModule::SendAddMailToShard( uint64 sendid, uint32 flag, uint64 recvid, const MapString& maildata )
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
            __LOG_ERROR__( "sendid={} mail={} failed!", sendid, strdata );
        }

        return ok;
    }

    bool KFMailClientModule::SendMail( uint32 configid, const KFElements* kfelements )
    {
        auto kfsetting = _kf_mail_config->FindMailSetting( configid );
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
        auto kfsetting = _kf_mail_config->FindMailSetting( configid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( nullptr, kfsetting, kfelements );
        return SendAddMailToShard( _invalid_int, KFMsg::PersonMail, recvid, maildata );
    }

    bool KFMailClientModule::SendMail( KFEntity* player, uint64 recvid, uint32 configid, const KFElements* kfelements )
    {
        auto kfsetting = _kf_mail_config->FindMailSetting( configid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( player, kfsetting, kfelements );
        return SendAddMailToShard( player->GetKeyID(), KFMsg::PersonMail, recvid, maildata );
    }
}

