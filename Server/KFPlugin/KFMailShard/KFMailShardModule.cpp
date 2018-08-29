#include "KFMailShardModule.h"

namespace KFrame
{
    //#define __USE_WORKER__

#ifdef __USE_WORKER__
    #define __REGISTER_MAIL_MESSAGE__ __REGISTER_WORKER_MESSAGE__
    #define __UNREGISTER_MAIL_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
    #define __MAIL_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( mail ) )
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
    #define __REGISTER_MAIL_MESSAGE__ __REGISTER_MESSAGE__
    #define __UNREGISTER_MAIL_MESSAGE__ __UNREGISTER_MESSAGE__
    static KFRedisDriver* _kf_redis_driver = nullptr;
    #define __MAIL_REDIS_DRIVER__ _kf_redis_driver
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

    KFMailShardModule::KFMailShardModule()
    {

    }

    KFMailShardModule::~KFMailShardModule()
    {

    }

    void KFMailShardModule::InitModule()
    {

    }

    void KFMailShardModule::BeforeRun()
    {
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ, &KFMailShardModule::HandleDeleteMailReq );
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ, &KFMailShardModule::HandleUpdateMailFlagReq );
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ, &KFMailShardModule::HandleNewPlayerLoginMailReq );
        __REGISTER_MAIL_MESSAGE__( KFMsg::S2S_GM_ADD_MAIL_REQ, &KFMailShardModule::HandleGMAddMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailShardModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_DELETE_MAIL_REQ );
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ );
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_FLAG_REQ );
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_NEW_PLAYER_LOGIN_MAIL_REQ );
        __UNREGISTER_MAIL_MESSAGE__( KFMsg::S2S_GM_ADD_MAIL_REQ );
    }

    void KFMailShardModule::OnceRun()
    {
#ifndef __USE_WORKER__
        // 初始化redis
        _kf_redis_driver = _kf_redis->CreateExecute( __KF_STRING__( mail ) );
#endif

        auto cleartime = _kf_option->GetValue<uint32>( "wholemailcleartime" );

        // 每天5点 清理过期的全局邮件
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, cleartime );
        _kf_schedule->RegisterSchedule( kfsetting, this, &KFMailShardModule::OnScheduleClearWholeOverdueMail );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMailShardModule::OnScheduleClearWholeOverdueMail( uint32 id, const char* data, uint32 size )
    {
        auto redisdriver = __MAIL_REDIS_DRIVER__;

        auto kfresult = redisdriver->QueryList( "zrangebyscore {} -inf +inf",
                                                __KF_STRING__( wholemail ) );
        if ( kfresult->_value.empty() )
        {
            return;
        }

        ListString overduelist;
        for ( auto& strmailid : kfresult->_value )
        {
            auto stringresult = redisdriver->QueryString( "hget {}:{} {}",
                                __KF_STRING__( mail ), strmailid, __KF_STRING__( id ) );
            if ( stringresult->IsOk() && stringresult->_value == _invalid_str )
            {
                overduelist.push_back( strmailid );
            }
        }

        if ( !overduelist.empty() )
        {
            redisdriver->Update( overduelist, "zrem {}", __KF_STRING__( wholemail ) );
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
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] mailtype[{}] error!", playerid, mailtype );
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

        auto redisdriver = __MAIL_REDIS_DRIVER__;
        auto mapresult = redisdriver->QueryMap( "hgetall {}", maillistkey );
        if ( mapresult->_value.empty() )
        {
            return;
        }

        MapString mailist;
        mailist.swap( mapresult->_value );


        // 最大邮件数量
        auto maxmailcount = _kf_option->GetValue<uint32>( "maxmailcount", kfmsg.mailtype() );

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg.playerid() );
        auto pbmails = ack.mutable_mails();
        ack.set_mailtype( kfmsg.mailtype() );

        VectorString overduelist;
        for ( auto& mailiter : mailist )
        {
            auto& strmailid = mailiter.first;

            // 这边是已经下发的邮件
            if ( KFUtility::ToValue<uint64>( strmailid ) <= kfmsg.maxmailid() )
            {
                continue;
            }


            auto kfresult = redisdriver->QueryMap( "hgetall {}:{}",
                                                   __KF_STRING__( mail ), strmailid );
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
            redisdriver->Update( overduelist, "zrem {}", maillistkey );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddMailReq );

        auto maillistkey = FormatMailKeyName( kfmsg.playerid(), kfmsg.mailtype(), __FUNC_LINE__ );
        if ( maillistkey.empty() )
        {
            return;
        }

        MapString maildata;
        auto pbmail = &kfmsg.pbmail();
        for ( auto i = 0; i < pbmail->data_size(); ++i )
        {
            auto pbdata = &pbmail->data( i );
            maildata[ pbdata->name() ] = pbdata->value();
        }

        // 添加邮件
        auto ok = AddMail( maillistkey, maildata );
        if ( !ok )
        {
            auto strmaildata = kfmsg.DebugString();
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] add mail[{}] failed!", kfmsg.playerid(), strmaildata );
        }
    }

    bool KFMailShardModule::AddMail( const std::string& maillistkey, MapString& maildata )
    {
        auto redisdriver = __MAIL_REDIS_DRIVER__;

        // 创建一个邮件id
        auto uint64result = redisdriver->QueryUInt64( "incr {}",
                            __KF_STRING__( mailidcreater ) );
        if ( uint64result->_value == _invalid_int )
        {
            return false;
        }
        /*
        if ( maildata[__KF_STRING__( flag )].empty() )
        {
            maildata[__KF_STRING__( flag )] = "0";
        }*/

        maildata[ __KF_STRING__( id ) ] = __TO_STRING__( uint64result->_value );
        maildata[ __KF_STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        // 添加邮件
        redisdriver->Append( maildata, "hmset {}:{}", __KF_STRING__( mail ), uint64result->_value );

        auto validtime = KFUtility::ToValue< uint32 >( maildata[ __KF_STRING__( validtime ) ] );
        if ( validtime != _invalid_int )
        {
            redisdriver->Append( "expire {}:{} {}", __KF_STRING__( mail ), uint64result->_value, validtime );
        }

        if ( maillistkey == _invalid_str )
        {
            redisdriver->Append( "zadd {} {} {}", __KF_STRING__( wholemail ), uint64result->_value, uint64result->_value );
        }
        else
        {
			// 非系统邮件
			redisdriver->Append( "hset {} {} {}", maillistkey, uint64result->_value, KFMsg::FlagEnum::Init );
        }

        auto kfresult = redisdriver->Pipeline();
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

        auto redisdriver = __MAIL_REDIS_DRIVER__;
        redisdriver->Append( "hdel {} {}", maillistkey, mailid );

        if ( mailtype != KFMsg::MailEnum::WholeMail )
        {
            redisdriver->Append( "del {}:{}", __KF_STRING__( mail ), mailid );
        }

        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "player[{}] del mail[{}:{}] failed!", playerid, mailtype, mailid );
        }

        return kfresult->IsOk();
    }

    void KFMailShardModule::LoadWholeMailToPerson( uint32 playerid )
    {
        auto redisdriver = __MAIL_REDIS_DRIVER__;

        // 获取玩家已经加载的最近一封GM邮件id
        auto stringresult = redisdriver->QueryString( "hget {}:{} {}",
                            __KF_STRING__( mailsendinfo ), playerid, __KF_STRING__( gmemaillastid ) );
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
        auto listresult = redisdriver->QueryList( "zrangebyscore {} ({} +inf",
                          __KF_STRING__( wholemail ), maxmailid );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto maillistkey = FormatMailKeyName( playerid, KFMsg::MailEnum::WholeMail, __FUNC_LINE__ );
        for ( auto& strmailid : listresult->_value )
        {
            redisdriver->Append( "hset {} {} {}", maillistkey, strmailid, KFMsg::FlagEnum::Init );
        }

        auto& newmaxmailid = listresult->_value.back();
        redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( mailsendinfo ), playerid, __KF_STRING__( gmemaillastid ), newmaxmailid );
        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "reload gm mail[{}] failed!", playerid );
        }
    }

    bool KFMailShardModule::UpdateMailFlag( uint32 playerid, uint64 mailid, uint32 mailtype, uint32 flag )
    {
        auto redisdriver = __MAIL_REDIS_DRIVER__;

        auto uint32result = redisdriver->QueryString( "hget {}:{} {}",
                            __KF_STRING__( mail ), mailid, __KF_STRING__( flag ) );
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
                auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
                                                      __KF_STRING__( mail ), mailid, __KF_STRING__( flag ), flag );
                if ( !kfresult->IsOk() )
                {
                    __LOG_ERROR__( KFLogEnum::Logic, "mailid[{}:{}] player[{}] flag[{}] failed!", mailtype, mailid, playerid, flag );
                }
            }

            auto maillistkey = FormatMailKeyName( playerid, mailtype, __FUNC_LINE__ );
            if ( !maillistkey.empty() )
            {
                ok = redisdriver->Execute( "hset {} {} {}", maillistkey, mailid, flag );
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
        auto redisdriver = __MAIL_REDIS_DRIVER__;

        // 最大邮件id
        auto listresult = redisdriver->QueryList( "zrevrange {} 0 0", __KF_STRING__( wholemail ) );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto& strmaxmailid = listresult->_value.front();
        auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
                                              __KF_STRING__( mailsendinfo ), kfmsg.playerid(),
                                              __KF_STRING__( gmemaillastid ), strmaxmailid );
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( KFLogEnum::Logic, "playerid[{}] mail[{}] failed!", kfmsg.playerid(), strmaxmailid );
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
            for ( auto i = 0; i < kfmsg.playerids().playerid_size(); ++i )
            {
                auto playerid = kfmsg.playerids().playerid( i );
                if ( _invalid_int == playerid )
                {
                    continue;
                }
                auto maillistkey = FormatMailKeyName( playerid, kfmsg.mailtype(), __FUNC_LINE__ );
                if ( maillistkey.empty() )
                {
                    return;
                }

                auto ok = AddMail( maillistkey, maildata );
                if ( !ok )
                {
                    auto strmaildata = kfmsg.DebugString();
                    __LOG_ERROR__( KFLogEnum::GM, "player[{}] add mail[{}] failed!", playerid, strmaildata );
                }

            }
        }
        else
        {
            auto ok = AddMail( _invalid_str, maildata );
            if ( !ok )
            {
                auto strmaildata = kfmsg.DebugString();
                __LOG_ERROR__( KFLogEnum::GM, " add mail[{}] failed!", strmaildata );
            }
        }

    }
}