#include "KFMailClientModule.h"
#include "KFMailConfig.h"

namespace KFrame
{
    KFMailClientModule::KFMailClientModule()
    {
    }

    KFMailClientModule::~KFMailClientModule()
    {

    }

    void KFMailClientModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_mail_config, true );
    }

    void KFMailClientModule::BeforeRun()
    {

        _kf_player->RegisterEnterFunction( this, &KFMailClientModule::OnEnterQueryMail );
        _kf_player->RegisterLeaveFunction( this, &KFMailClientModule::OnLeaveQueryMail );
        _kf_player->RegisterNewPlayerFunction( this, &KFMailClientModule::OnNewPlayerHandleGMMail );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_QUERY_MAIL_REQ, &KFMailClientModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK, &KFMailClientModule::HandleQueryMailAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_VIEW_MAIL_REQ, &KFMailClientModule::HandleViewMailReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_DELETE_MAIL_REQ, &KFMailClientModule::HandleDeleteMailReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_MAIL_REWARD_REQ, &KFMailClientModule::HandleReceiveMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_ACK, &KFMailClientModule::HandleUpdateMailFlagAck );
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
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_VIEW_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DELETE_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_RECEIVE_MAIL_REWARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NOTICE_NEW_MAIL_REQ );
    }

    bool KFMailClientModule::SendMessageToMail( uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_cluster->SendToShard( __KF_STRING__( mail ), msgid, message );
    }

    void KFMailClientModule::OnEnterQueryMail( KFEntity* player )
    {
        auto playerid = player->GetKeyID();
        __REGISTER_LOOP_TIMER__( playerid, KFTimeEnum::OneMinuteMicSecond * 5, &KFMailClientModule::OnTimerQueryMail );
    }

    void KFMailClientModule::OnLeaveQueryMail( KFEntity* player )
    {
        auto playerid = player->GetKeyID();
        __UNREGISTER_OBJECT_TIMER__( playerid );
    }

    void KFMailClientModule::OnNewPlayerHandleGMMail( KFEntity* player )
    {
        KFMsg::S2SNewPlayerLoginMailReq req;
        req.set_playerid( player->GetKeyID() );
        SendMessageToMail( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ, &req );
    }

    __KF_TIMER_FUNCTION__( KFMailClientModule::OnTimerQueryMail )
    {
        auto playerid = static_cast< uint32 >( objectid );
        auto player = _kf_player->FindPlayer( playerid, __FUNC_LINE__ );
        if ( player == nullptr )
        {
            return;
        }

        // 统一逻辑, 分多次查询不同类型邮件
        for ( uint32 i = KFMsg::MailEnum_MIN; i <= KFMsg::MailEnum_MAX; ++i )
        {
            // 获得该类型最大的邮件id
            auto maxmailid = FindMaxMailId( player, i );
            SendQueryMailMessage( playerid, i, maxmailid );
        }
    }

    void KFMailClientModule::SendQueryMailMessage( uint32 playerid, uint32 mailtype, uint64 maxmailid )
    {
        KFMsg::S2SQueryMailReq req;
        req.set_playerid( playerid );
        req.set_mailtype( mailtype );
        req.set_maxmailid( maxmailid );
        SendMessageToMail( KFMsg::S2S_QUERY_MAIL_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgQueryMailReq );
        if ( KFMsg::MailEnum_MIN > kfmsg.mailtype() || KFMsg::MailEnum_MAX < kfmsg.mailtype() )
        {
            return;
        }
        auto maxmailid = FindMaxMailId( player, kfmsg.mailtype() );
        SendQueryMailMessage( playerid, kfmsg.mailtype(), maxmailid );
    }

    uint64 KFMailClientModule::FindMaxMailId( KFEntity* player, uint32 mailtype )
    {
        auto kfobject = player->GetData();
        auto kfmailrecord = kfobject->FindData( __KF_STRING__( mail ) );

        auto maxmailid = _invalid_int;

        auto kfmail = kfmailrecord->FirstData();
        while ( kfmail != nullptr )
        {
            auto type = kfmail->GetValue< uint32 >( __KF_STRING__( type ) );
            if ( type == mailtype )
            {
                auto mailid = kfmail->GetValue< uint64 >( __KF_STRING__( id ) );
                if ( mailid > maxmailid )
                {
                    maxmailid = mailid;
                }
            }

            kfmail = kfmailrecord->NextData();
        }

        return maxmailid;
    }

    uint64 KFMailClientModule::FindMinMailId( KFEntity* player, uint32 mailtype )
    {
        auto kfobject = player->GetData();
        auto kfmailrecord = kfobject->FindData( __KF_STRING__( mail ) );

        auto minmailid = std::numeric_limits<uint64>::max();

        auto kfmail = kfmailrecord->FirstData();
        while ( kfmail != nullptr )
        {
            auto type = kfmail->GetValue< uint32 >( __KF_STRING__( type ) );
            if ( type == mailtype )
            {
                auto mailid = kfmail->GetValue< uint64 >( __KF_STRING__( id ) );
                if ( mailid < minmailid )
                {
                    minmailid = mailid;
                }
            }

            kfmail = kfmailrecord->NextData();
        }

        return minmailid;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleQueryMailAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SQueryMailAck );

        auto kfobject = player->GetData();
        auto kfmailrecord = kfobject->FindData( __KF_STRING__( mail ) );

        // 将邮件保存到玩家属性中
        auto pbmails = &kfmsg.mails();
        for ( auto i = 0; i < pbmails->mail_size(); ++i )
        {
            auto pbmail = &pbmails->mail( i );
            auto kfmail = ParsePBMailToData( pbmail, kfmailrecord->GetDataSetting() );
            if ( kfmail == nullptr )
            {
                continue;
            }

            player->AddData( kfmailrecord, kfmail );
        }
    }

    KFData* KFMailClientModule::ParsePBMailToData( const KFMsg::PBMail* pbmail, const KFDataSetting* kfsetting )
    {
        auto kfmail = _kf_kernel->CreateObject( kfsetting );
        if ( kfmail == nullptr )
        {
            return nullptr;
        }

        // 设置邮件属性
        for ( auto i = 0; i < pbmail->data_size(); ++i )
        {
            auto pbdata = &pbmail->data( i );
            kfmail->SetValue< std::string >( pbdata->name(), pbdata->value() );
        }

        return kfmail;
    }

    bool KFMailClientModule::CheckMailTimeOut( KFData* kfmail )
    {
        auto validtime = kfmail->GetValue<uint32>( __KF_STRING__( validtime ) );
        auto sendtime = kfmail->GetValue<uint64>( __KF_STRING__( sendtime ) );

        return sendtime + validtime < KFGlobal::Instance()->_real_time;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleViewMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgViewMailReq );

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.mailid() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        if ( CheckMailTimeOut( kfmail ) )
        {
            return _kf_display->SendToClient( player, KFMsg::MailTimeOut );
        }

        auto flag = kfmail->GetValue< uint32>( __KF_STRING__( flag ) );
        if ( flag != KFMsg::FlagEnum::Init )
        {
            return;
        }

        // 更新状态
        UpdateFlagToMail( playerid, kfmail, KFMsg::FlagEnum::Done );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleDeleteMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDeleteMailReq );

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.mailid() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        // 如果有奖励, 并且没有领取
        auto flag = kfmail->GetValue< uint32 >( __KF_STRING__( flag ) );
        auto reward = kfmail->GetValue< std::string >( __KF_STRING__( reward ) );
        if ( flag != KFMsg::FlagEnum::Received && !reward.empty() )
        {
            return _kf_display->SendToClient( player, KFMsg::MailDeleteFailed );
        }

        // 更新到邮件集群
        UpdateFlagToMail( playerid, kfmail, KFMsg::FlagEnum::Remove );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleReceiveMailReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgReceiveMailRewardReq );

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), kfmsg.mailid() );
        if ( kfmail == nullptr )
        {
            return _kf_display->SendToClient( player, KFMsg::MailNotExist );
        }

        auto flag = kfmail->GetValue< uint32 >( __KF_STRING__( flag ) );
        if ( flag == KFMsg::FlagEnum::Received )
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

        auto mailtype = kfmail->GetValue< uint32 >( __KF_STRING__( type ) );
        auto deltype = _kf_option->GetUInt32( __KF_STRING__( mailreceiveremove ), mailtype );
        if ( deltype == 1 )
        {
            UpdateFlagToMail( playerid, kfmail, KFMsg::FlagEnum::ReceiveRemove );
        }
        else
        {
            UpdateFlagToMail( playerid, kfmail, KFMsg::FlagEnum::Received );
        }
    }

    bool KFMailClientModule::UpdateFlagToMail( uint32 playerid, KFData* kfmail, uint32 flag )
    {
        auto mailid = kfmail->GetKeyID();
        auto mailtype = kfmail->GetValue< uint32 >( __KF_STRING__( type ) );

        KFMsg::S2SUpdateMailFlagReq req;
        req.set_playerid( playerid );
        req.set_mailid( mailid );
        req.set_mailtype( mailtype );
        req.set_flag( flag );
        auto ok = SendMessageToMail( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ, &req );
        if ( !ok )
        {
            auto strmailid = __TO_STRING__( mailid );
            __LOG_ERROR__( "player[{}] update mail[{}:{}] flag[{}] failed!",
                           playerid, mailtype, strmailid, flag );
        }

        return ok;
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleUpdateMailFlagAck )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SUpdateMailFlagAck );

        switch ( kfmsg.flag() )
        {
        case KFMsg::FlagEnum::Done:
        {
            player->UpdateData( __KF_STRING__( mail ), kfmsg.mailid(), __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Done );
        }
        break;
        case KFMsg::FlagEnum::Remove:
        {
            player->RemoveData( __KF_STRING__( mail ), kfmsg.mailid() );
        }
        break;
        case KFMsg::FlagEnum::Received:
        {
            ReceiveMailReward( player, kfmsg.mailid() );
        }
        break;
        case KFMsg::FlagEnum::ReceiveRemove:
        {
            ReceiveMailReward( player, kfmsg.mailid() );
            player->RemoveData( __KF_STRING__( mail ), kfmsg.mailid() );
        }
        break;
        default:
            break;
        }
    }

    void KFMailClientModule::ReceiveMailReward( KFEntity* player, uint64 mailid )
    {
        auto strmailid = __TO_STRING__( mailid );

        auto kfobject = player->GetData();
        auto kfmail = kfobject->FindData( __KF_STRING__( mail ), mailid );
        if ( kfmail == nullptr )
        {
            return __LOG_ERROR__( "player[{}] can't find mail[{}]!", player->GetKeyID(), strmailid );
        }

        auto reward = kfmail->GetValue< std::string >( __KF_STRING__( reward ) );
        if ( reward.empty() )
        {
            return __LOG_ERROR__( "player[{}] mail[{}] no reward!", player->GetKeyID(), strmailid );
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( reward, __FUNC_LINE__ );
        if ( !ok )
        {
            return __LOG_ERROR__( "player[{}] mail[{}] reward[{}] error!", player->GetKeyID(), strmailid, reward );
        }

        player->UpdateData( kfmail, __KF_STRING__( flag ), KFOperateEnum::Set, KFMsg::FlagEnum::Received );
        player->AddAgentData( &kfagents, 1.0f, true, __FUNC_LINE__ );

        // 如果有配置回复邮件id, 回复邮件
        auto configid = kfmail->GetValue< uint32 >( __KF_STRING__( configid ) );
        auto kfsetting = _kf_mail_config->FindMailSetting( configid );
        if ( kfsetting != nullptr && kfsetting->_reply_id != _invalid_int )
        {
            auto senderid = kfmail->GetValue< uint32 >( __KF_STRING__( senderid ) );
            auto serverid = kfmail->GetValue< uint32 >( __KF_STRING__( serverid ) );
            SendMail( player, serverid, senderid, kfsetting->_reply_id, reward );
        }
    }

    MapString& KFMailClientModule::FormatMailData( KFEntity* sender, const KFMailSetting* kfsetting, const KFAgents* kfagents, const std::string& extend )
    {
        // 有优化的空间, 服务器只发configid到客户端, 客户端根据邮件配置表来获得邮件的基础数据
        // 暂时先发给客户端, 省去客户端读取邮件配置表
        static MapString _mail_data;
        _mail_data.clear();

        // 配置id
        _mail_data.insert( std::make_pair( __KF_STRING__( configid ), __TO_STRING__( kfsetting->_config_id ) ) );

        // 类型
        _mail_data.insert( std::make_pair( __KF_STRING__( type ), __TO_STRING__( kfsetting->_type ) ) );

        // 标识
        _mail_data.insert( std::make_pair( __KF_STRING__( flag ), "0" ) );

        // 标题
        _mail_data.insert( std::make_pair( __KF_STRING__( title ), kfsetting->_title ) );

        // 内容
        _mail_data.insert( std::make_pair( __KF_STRING__( content ), kfsetting->_content ) );

        // 有效时间
        _mail_data.insert( std::make_pair( __KF_STRING__( validtime ), __TO_STRING__( kfsetting->_valid_time ) ) );

        // 附加信息
        if ( !extend.empty() )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( extend ), extend ) );
        }
        else if ( !kfsetting->_extend.empty() )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( extend ), kfsetting->_extend ) );
        }

        // 奖励
        if ( kfagents != nullptr )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( reward ), kfagents->_string ) );
        }
        else if ( !kfsetting->_rewards._string.empty() )
        {
            _mail_data.insert( std::make_pair( __KF_STRING__( reward ), kfsetting->_rewards._string ) );
        }

        // 发送者信息
        if ( sender != nullptr )
        {
            auto kfobject = sender->GetData();
            auto kfbasic = kfobject->FindData( __KF_STRING__( basic ) );

            // 发送者名字
            _mail_data.insert( std::make_pair( __KF_STRING__( sendername ), kfbasic->GetValue< std::string >( __KF_STRING__( name ) ) ) );

            // 发送者id
            _mail_data.insert( std::make_pair( __KF_STRING__( senderid ), __TO_STRING__( kfobject->GetKeyID() ) ) );

            // 发送者服务器id
            _mail_data.insert( std::make_pair( __KF_STRING__( serverid ), __TO_STRING__( kfbasic->GetValue< uint32 >( __KF_STRING__( serverid ) ) ) ) );

            // 头像
            _mail_data.insert( std::make_pair( __KF_STRING__( icon ), kfbasic->GetValue< std::string >( __KF_STRING__( icon ) ) ) );

            // 头像
            _mail_data.insert( std::make_pair( __KF_STRING__( iconbox ), kfbasic->GetValue< std::string >( __KF_STRING__( iconbox ) ) ) );

            // 性别
            _mail_data.insert( std::make_pair( __KF_STRING__( sex ), __TO_STRING__( kfbasic->GetValue< uint32 >( __KF_STRING__( sex ) ) ) ) );
        }

        return _mail_data;
    }

    bool KFMailClientModule::SendAddMailToCluster( uint32 serverid, uint32 playerid, uint32 mailtype, const MapString& maildata )
    {
        KFMsg::S2SAddMailReq req;
        req.set_mailtype( mailtype );
        req.set_playerid( playerid );

        auto pbmail = req.mutable_pbmail();
        for ( auto& iter : maildata )
        {
            auto pbdata = pbmail->add_data();
            pbdata->set_name( iter.first );
            pbdata->set_value( iter.second );
        }

        return SendMessageToMail( KFMsg::S2S_ADD_MAIL_REQ, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFMailClientModule::HandleNoticeNewMailReq )
    {
        __SERVER_PROTO_PARSE__( KFMsg::S2SNoticeNewMailReq );

        auto maxmailid = FindMaxMailId( player, kfmsg.mailtype() );
        SendQueryMailMessage( player->GetKeyID(), kfmsg.mailtype(), maxmailid );
    }

    bool KFMailClientModule::SendMail( KFEntity* player, uint32 mailconfigid, const KFAgents* kfagents )
    {
        auto kfsetting = _kf_mail_config->FindMailSetting( mailconfigid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( nullptr, kfsetting, kfagents, _invalid_str );
        return SendAddMailToCluster( KFGlobal::Instance()->_app_id, player->GetKeyID(), kfsetting->_type, maildata );
    }

    bool KFMailClientModule::SendMail( KFEntity* player, uint32 toserverid, uint32 toplayerid, uint32 mailconfigid, const KFAgents* kfagents )
    {
        auto kfsetting = _kf_mail_config->FindMailSetting( mailconfigid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( player, kfsetting, kfagents, _invalid_str  );

        return SendAddMailToCluster( toserverid, toplayerid, kfsetting->_type, maildata );
    }

    bool KFMailClientModule::SendMail( KFEntity* player, uint32 toserverid, uint32 toplayerid, uint32 mailconfigid, const std::string& extend )
    {
        auto kfsetting = _kf_mail_config->FindMailSetting( mailconfigid );
        if ( kfsetting == nullptr )
        {
            return false;
        }

        auto& maildata = FormatMailData( player, kfsetting, nullptr, extend );
        return SendAddMailToCluster( toserverid, toplayerid, kfsetting->_type, maildata );
    }
}

