#include "KFMailShardModule.h"

namespace KFrame
{
#define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendToClient( kfguid, msgid, message )

    void KFMailShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ, &KFMailShardModule::HandleDeleteMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ, &KFMailShardModule::HandleUpdateMailFlagReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ, &KFMailShardModule::HandleNewPlayerLoginMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_GM_ADD_MAIL_REQ, &KFMailShardModule::HandleGMAddMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailShardModule::BeforeShut()
    {
        __UNREGISTER_SCHEDULE_FUNCTION__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_GM_ADD_MAIL_REQ );
    }

    void KFMailShardModule::OnceRun()
    {
        // 初始化redis
        _mail_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( mail ) );
        _public_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( public ) );

        // 每天5点 清理过期的全局邮件
        auto cleartime = _kf_option->GetUInt32( __KF_STRING__( wholemailcleartime ) );
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, cleartime );
        __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFMailShardModule::OnScheduleClearWholeOverdueMail );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SCHEDULE_FUNCTION__( KFMailShardModule::OnScheduleClearWholeOverdueMail )
    {
        auto kfresult = _mail_redis_driver->QueryList( "zrangebyscore {} -inf +inf", __KF_STRING__( wholemail ) );
        if ( kfresult->_value.empty() )
        {
            return;
        }

        ListString overduelist;
        for ( auto& strmailid : kfresult->_value )
        {
            auto stringresult = _mail_redis_driver->QueryString( "hget {}:{} {}", __KF_STRING__( mail ), strmailid, __KF_STRING__( id ) );
            if ( stringresult->IsOk() && stringresult->_value == _invalid_str )
            {
                overduelist.push_back( strmailid );
            }
        }

        if ( !overduelist.empty() )
        {
            _mail_redis_driver->Update( overduelist, "zrem {}", __KF_STRING__( wholemail ) );
        }
    }

    std::string KFMailShardModule::FormatMailKeyName( uint32 playerid, uint32 mailtype, const char* function, uint32 line )
    {
        switch ( mailtype )
        {
        case  KFMsg::MailEnum::WholeMail:
            return __FORMAT__( "{}:{}", __KF_STRING__( wholemail ), playerid );
        case KFMsg::MailEnum::FriendMail:
            return __FORMAT__( "{}:{}", __KF_STRING__( personmailfriend ), playerid );
        case KFMsg::MailEnum::GiftMail:
            return __FORMAT__( "{}:{}", __KF_STRING__( personmailgift ), playerid );
        default:
            __LOG_ERROR__( "player[{}] mailtype[{}] error!", playerid, mailtype );
            break;
        }

        return _invalid_str;
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleQueryMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryMailReq );

        // 计算玩家的全局邮件信息
        if ( kfmsg.mailtype() == KFMsg::MailEnum::WholeMail )
        {
            LoadWholeMailToPerson( kfmsg.playerid() );
        }

        // 查询邮件列表
        auto maillistkey = FormatMailKeyName( kfmsg.playerid(), kfmsg.mailtype(), __FUNC_LINE__ );
        if ( maillistkey.empty() )
        {
            return;
        }

        auto mailkeylist = _mail_redis_driver->QueryMap( "hgetall {}", maillistkey );
        if ( mailkeylist->_value.empty() )
        {
            return;
        }

        // 最大邮件数量
        auto maxmailcount = _kf_option->GetUInt32( __KF_STRING__( mailmaxcount ), kfmsg.mailtype() );

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg.playerid() );
        auto pbmails = ack.mutable_mails();
        ack.set_mailtype( kfmsg.mailtype() );

        VectorString overduelist;
        for ( auto& mailiter : mailkeylist->_value )
        {
            auto& strmailid = mailiter.first;

            // 这边是已经下发的邮件
            auto mailid = KFUtility::ToValue<uint64>( strmailid );
            if ( mailid <= kfmsg.maxmailid() )
            {
                continue;
            }

            auto kfresult = _mail_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( mail ), strmailid );
            if ( !kfresult->IsOk() )
            {
                continue;
            }

            // 已经过期了
            if ( kfresult->_value.empty() )
            {
                overduelist.push_back( strmailid );
                continue;
            }

            auto pbmail = pbmails->add_mail();
            for ( auto& iter : kfresult->_value )
            {
                auto pbdata = pbmail->add_data();
                pbdata->set_name( iter.first );

                // 系统邮件做特殊处理
                if ( iter.first == __KF_STRING__( flag ) && kfmsg.mailtype() == KFMsg::MailEnum::WholeMail )
                {
                    pbdata->set_value( mailiter.second );
                }
                else
                {
                    pbdata->set_value( iter.second );
                }
            }

            if ( static_cast< uint32 >( pbmails->mail_size() ) >= maxmailcount )
            {
                break;
            }
        }

        if ( pbmails->mail_size() != 0 )
        {
            __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_QUERY_MAIL_ACK, &ack );
        }

        // 删除过期邮件
        if ( !overduelist.empty() )
        {
            _mail_redis_driver->Update( overduelist, "zrem {}", maillistkey );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddMailReq );

        MapString maildata;
        auto pbmail = &kfmsg.pbmail();
        for ( auto i = 0; i < pbmail->data_size(); ++i )
        {
            auto pbdata = &pbmail->data( i );
            maildata[ pbdata->name() ] = pbdata->value();
        }

        // 添加邮件
        auto ok = AddMail( kfmsg.playerid(), kfmsg.mailtype(), maildata );
        if ( !ok )
        {
            auto strmaildata = kfmsg.DebugString();
            __LOG_ERROR__( "player[{}] add mail[{}] failed!", kfmsg.playerid(), strmaildata );
        }
    }

    bool KFMailShardModule::AddMail( uint32 playerid, uint32 mailtype, MapString& maildata )
    {
        // 创建一个邮件id
        auto uint64result = _mail_redis_driver->QueryUInt64( "incr {}", __KF_STRING__( mailidcreater ) );
        if ( uint64result->_value == _invalid_int )
        {
            return false;
        }

        maildata[ __KF_STRING__( id ) ] = __TO_STRING__( uint64result->_value );
        maildata[ __KF_STRING__( flag ) ] = __TO_STRING__( _invalid_int );
        maildata[ __KF_STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        // 添加邮件
        _mail_redis_driver->Append( maildata, "hmset {}:{}", __KF_STRING__( mail ), uint64result->_value );

        auto validtime = KFUtility::ToValue< uint32 >( maildata[ __KF_STRING__( validtime ) ] );
        if ( validtime != _invalid_int )
        {
            _mail_redis_driver->Append( "expire {}:{} {}", __KF_STRING__( mail ), uint64result->_value, validtime );
        }

        if ( playerid == _invalid_int )
        {
            // 系统邮件
            _mail_redis_driver->Append( "zadd {} {} {}", __KF_STRING__( wholemail ), uint64result->_value, uint64result->_value );
        }
        else
        {
            // 非系统邮件
            auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNC_LINE__ );
            if ( !maillistkey.empty() )
            {
                _mail_redis_driver->Append( "hset {} {} {}", maillistkey, uint64result->_value, KFMsg::FlagEnum::Init );
            }
        }

        auto kfresult = _mail_redis_driver->Pipeline();
        if ( kfresult->IsOk() )
        {
            NoticePlayerNewMail( playerid, mailtype );
        }

        return kfresult->IsOk();
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleDeleteMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDeleteMailReq );

        RemoveMail( kfmsg.playerid(), kfmsg.mailtype(), kfmsg.mailid() );
    }

    bool KFMailShardModule::RemoveMail( uint32 playerid, uint32 mailtype, uint64 mailid )
    {
        auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNC_LINE__ );
        if ( maillistkey == _invalid_str )
        {
            return false;
        }

        _mail_redis_driver->Execute( "hdel {} {}", maillistkey, mailid );
        if ( mailtype != KFMsg::MailEnum::WholeMail )
        {
            _mail_redis_driver->Execute( "del {}:{}", __KF_STRING__( mail ), mailid );
        }

        return true;
    }

    void KFMailShardModule::LoadWholeMailToPerson( uint32 playerid )
    {
        // 获取玩家已经加载的最近一封GM邮件id
        auto stringresult = _mail_redis_driver->QueryString( "hget {}:{} {}", __KF_STRING__( mailsendinfo ), playerid, __KF_STRING__( gmemaillastid ) );
        if ( !stringresult->IsOk() )
        {
            return;
        }

        uint64 maxmailid = _invalid_int;
        if ( !stringresult->_value.empty() )
        {
            maxmailid = KFUtility::ToValue< uint64 >( stringresult->_value );
        }

        // 从全局GM邮件列表中取出玩家未获取的
        auto listresult = _mail_redis_driver->QueryList( "zrangebyscore {} ({} +inf", __KF_STRING__( wholemail ), maxmailid );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto maillistkey = FormatMailKeyName( playerid, KFMsg::MailEnum::WholeMail, __FUNC_LINE__ );
        for ( auto& strmailid : listresult->_value )
        {
            _mail_redis_driver->Append( "hset {} {} {}", maillistkey, strmailid, KFMsg::FlagEnum::Init );
        }

        auto& newmaxmailid = listresult->_value.back();
        _mail_redis_driver->Append( "hset {}:{} {} {}", __KF_STRING__( mailsendinfo ), playerid, __KF_STRING__( gmemaillastid ), newmaxmailid );
        auto kfresult = _mail_redis_driver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "reload gm mail[{}] failed!", playerid );
        }
    }

    bool KFMailShardModule::UpdateMailFlag( uint32 playerid, uint64 mailid, uint32 mailtype, uint32 flag )
    {
        auto uint32result = _mail_redis_driver->QueryString( "hget {}:{} {}", __KF_STRING__( mail ), mailid, __KF_STRING__( flag ) );
        if ( !uint32result->IsOk() || uint32result->_value == _invalid_str )
        {
            return false;
        }

        auto ok = false;
        switch ( flag )
        {
        case KFMsg::FlagEnum::Remove:
        case KFMsg::FlagEnum::ReceiveRemove:
        {
            ok = RemoveMail( playerid, mailtype, mailid );
        }
        break;
        case KFMsg::FlagEnum::Done:
        case KFMsg::FlagEnum::Received:
        {
            if ( mailtype != KFMsg::MailEnum::WholeMail )
            {
                auto kfresult = _mail_redis_driver->Execute( "hset {}:{} {} {}", __KF_STRING__( mail ), mailid, __KF_STRING__( flag ), flag );
                if ( !kfresult->IsOk() )
                {
                    __LOG_ERROR__( "mailid[{}:{}] player[{}] flag[{}] failed!", mailtype, mailid, playerid, flag );
                }
            }

            auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNC_LINE__ );
            if ( !maillistkey.empty() )
            {
                ok = _mail_redis_driver->Execute( "hset {} {} {}", maillistkey, mailid, flag );
            }
        }
        break;
        default:
            break;
        }

        return ok;
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleUpdateMailFlagReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateMailFlagReq );

        auto ok = UpdateMailFlag( kfmsg.playerid(), kfmsg.mailid(), kfmsg.mailtype(), kfmsg.flag() );
        if ( !ok )
        {
            return;
        }

        KFMsg::S2SUpdateMailFlagAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_flag( kfmsg.flag() );
        ack.set_mailid( kfmsg.mailid() );
        ack.set_mailtype( kfmsg.mailtype() );
        __SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_UPDATE_MAIL_FLAG_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleNewPlayerLoginMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SNewPlayerLoginMailReq );

        // 最大邮件id
        auto listresult = _mail_redis_driver->QueryList( "zrevrange {} 0 0", __KF_STRING__( wholemail ) );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto& strmaxmailid = listresult->_value.front();
        auto kfresult = _mail_redis_driver->Execute( "hset {}:{} {} {}", __KF_STRING__( mailsendinfo ), kfmsg.playerid(), __KF_STRING__( gmemaillastid ), strmaxmailid );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "playerid[{}] mail[{}] failed!", kfmsg.playerid(), strmaxmailid );
        }
    }


    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleGMAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGMAddMailReq );

        MapString maildata;
        auto pbmail = &kfmsg.pbmail();
        for ( auto i = 0; i < pbmail->data_size(); ++i )
        {
            auto pbdata = &pbmail->data( i );
            maildata[pbdata->name()] = pbdata->value();
        }

        if ( kfmsg.has_playerids() )
        {
            auto pbplayerids = &kfmsg.playerids();
            for ( auto i = 0; i < pbplayerids->playerid_size(); ++i )
            {
                auto playerid = pbplayerids->playerid( i );
                auto ok = AddMail( playerid, kfmsg.mailtype(), maildata );
                if ( !ok )
                {
                    auto strmaildata = kfmsg.DebugString();
                    __LOG_ERROR__( "player[{}] add mail[{}] failed!", playerid, strmaildata );
                }
            }
        }
        else
        {
            auto ok = AddMail( _invalid_int, _invalid_int, maildata );
            if ( !ok )
            {
                auto strmaildata = kfmsg.DebugString();
                __LOG_ERROR__( " add mail[{}] failed!", strmaildata );
            }
        }
    }

    void KFMailShardModule::NoticePlayerNewMail( uint32 playerid, uint32 mailtype )
    {
        if ( playerid == _invalid_int )
        {
            return;
        }

        auto kfresult = _public_redis_driver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), playerid, __KF_STRING__( serverid ) );
        if ( !kfresult->IsOk() || kfresult->_value == _invalid_int )
        {
            return;
        }

        KFMsg::S2SNoticeNewMailReq notice;
        notice.set_playerid( playerid );
        notice.set_mailtype( mailtype );
        _kf_cluster_shard->SendToPlayer( kfresult->_value, playerid, KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &notice );
    }
}