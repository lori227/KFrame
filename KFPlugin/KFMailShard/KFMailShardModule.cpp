#include "KFMailShardModule.hpp"

namespace KFrame
{
    void KFMailShardModule::BeforeRun()
    {
        __REGISTER_HTTP__( __STRING__( addmail ), true, &KFMailShardModule::HandleGMAddMailReq );
        __REGISTER_HTTP__( __STRING__( delmail ), true, &KFMailShardModule::HandleGMDeleteMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_REQ, &KFMailShardModule::HandleUpdateMailStatusReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_NEW_PLAYER_MAIL_REQ, &KFMailShardModule::HandleNewPlayerMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailShardModule::BeforeShut()
    {
        __UN_SCHEDULE__();
        __UN_HTTP__( __STRING__( addmail ) );
        __UN_HTTP__( __STRING__( delmail ) );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UN_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ );
        __UN_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_REQ );
        __UN_MESSAGE__( KFMsg::S2S_NEW_PLAYER_MAIL_REQ );
    }

    void KFMailShardModule::PrepareRun()
    {
        // 初始化redis
        _mail_driver = _kf_redis->Create( __STRING__( mail ) );
        _auth_driver = _kf_redis->Create( __STRING__( auth ) );

        // 每天5点 清理过期的全局邮件
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, 5 );
        __REGISTER_SCHEDULE__( kfsetting, &KFMailShardModule::OnScheduleClearWholeOverdueMail );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMAddMailReq )
    {
        auto strdata = data;
        KFUtility::ReplaceString( strdata, " ", "" );
        KFUtility::ReplaceString( strdata, "%", "" );

        __JSON_PARSE_STRING__( request, strdata );

        // 邮件玩家列表
        UInt64Set playerlist;
        if ( __JSON_HAS_MEMBER__( request, __STRING__( playerid ) ) )
        {
            auto strplayerid = __JSON_GET_STRING__( request, __STRING__( playerid ) );
            while ( !strplayerid.empty() )
            {
                auto playerid = KFUtility::SplitValue( strplayerid, "," );
                if ( playerid != _invalid_int )
                {
                    playerlist.insert( playerid );
                }
            }
            __JSON_REMOVE__( request, __STRING__( playerid ) );
        }

        // 小区列表
        UInt64Set zonelist;
        if ( __JSON_HAS_MEMBER__( request, __STRING__( zoneid ) ) )
        {
            auto strzoneid = __JSON_GET_STRING__( request, __STRING__( zoneid ) );
            while ( !strzoneid.empty() )
            {
                auto zoneid = KFUtility::SplitValue( strzoneid, "," );
                if ( zoneid != _invalid_int )
                {
                    zonelist.insert( zoneid );
                }
            }
            __JSON_REMOVE__( request, __STRING__( zoneid ) );
        }

        // 邮件内容
        StringMap values;
        __JSON_TO_MAP__( request, values );

        if ( playerlist.empty() )
        {
            for ( auto zoneid : zonelist )
            {
                AddMail( KFMsg::GlobalMail, zoneid, values );
            }
        }
        else
        {
            for ( auto playerid : playerlist )
            {
                AddMail( KFMsg::PersonMail, playerid, values );
            }
        }

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMDeleteMailReq )
    {
        __JSON_PARSE_STRING__( request, data );

        const auto& maillist = __JSON_GET_ARRRY__( request, __STRING__( mailid ) );
        auto count = __JSON_ARRAY_SIZE__( maillist );
        for ( auto i = 0u; i < count; ++i )
        {
            auto mailid = __JSON_ARRAY_INDEX__( maillist, i ).GetUint64();
            _mail_driver->Append( "del {}:{}", __STRING__( mail ), mailid );
        }
        _mail_driver->Pipeline();

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_SCHEDULE_FUNCTION__( KFMailShardModule::OnScheduleClearWholeOverdueMail )
    {
        // 查询列表
        auto kflist = _mail_driver->QueryList( "smembers {}", __STRING__( mailzonelist ) );
        if ( kflist->_value.empty() )
        {
            return;
        }

        // 清空过期的全局邮件列表
        for ( auto zoneid : kflist->_value )
        {
            auto globalmailkey = __FORMAT__( "{}:{}", __STRING__( globalmail ), zoneid );

            auto kfresult = _mail_driver->QueryList( "zrangebyscore {} -inf +inf", globalmailkey );
            if ( kfresult->_value.empty() )
            {
                continue;
            }

            StringList overduelist;
            for ( auto& strmailid : kfresult->_value )
            {
                auto stringresult = _mail_driver->QueryString( "hget {}:{} {}", __STRING__( mail ), strmailid, __STRING__( id ) );
                if ( stringresult->IsOk() && stringresult->_value == _invalid_string )
                {
                    overduelist.push_back( strmailid );
                }
            }

            if ( !overduelist.empty() )
            {
                _mail_driver->Update( overduelist, "zrem {}", globalmailkey );
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMailShardModule::LoadGlobalMailToPerson( uint64 playerid, uint32 zoneid )
    {
        auto mailinfokey = __FORMAT__( "{}:{}:{}", __STRING__( mailinfo ), playerid, zoneid );

        // 获取玩家已经加载的最近一封GM邮件id
        auto kfmailid = _mail_driver->QueryUInt64( "hget {} {}", mailinfokey, __STRING__( gmmailid ) );
        if ( !kfmailid->IsOk() )
        {
            return;
        }

        // 查询全局邮件列表
        auto listresult = _mail_driver->QueryList( "zrangebyscore {}:{} ({} +inf", __STRING__( globalmail ), zoneid, kfmailid->_value );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto& newmaxmailid = listresult->_value.back();
        _mail_driver->Append( "hset {} {} {}", mailinfokey, __STRING__( gmmailid ), newmaxmailid );

        // 添加到个人邮件列表中
        for ( auto& strmailid : listresult->_value )
        {
            _mail_driver->Append( "hset {}:{} {} {}", __STRING__( maillist ), playerid, strmailid, KFMsg::InitStatus );
        }

        auto kfresult = _mail_driver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "reload gm mail[{}] failed", playerid );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleQueryMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryMailReq );

        // 全局邮件信息
        LoadGlobalMailToPerson( kfmsg.playerid(), _invalid_int );

        // 小区邮件
        LoadGlobalMailToPerson( kfmsg.playerid(), kfmsg.zoneid() );

        // 查询邮件列表
        auto mailkeylist = _mail_driver->QueryMap( "hgetall {}:{}", __STRING__( maillist ), kfmsg.playerid() );
        if ( mailkeylist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg.playerid() );

        StringList overduelist;
        for ( auto& mailiter : mailkeylist->_value )
        {
            auto& strmailid = mailiter.first;

            // 判断是否已经下发的邮件
            auto mailid = __TO_UINT64__( strmailid );
            if ( mailid <= kfmsg.maxid() )
            {
                continue;
            }

            auto kfresult = _mail_driver->QueryMap( "hgetall {}:{}", __STRING__( mail ), strmailid );
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

            // 邮件属性
            auto pbmail = ack.add_mail()->mutable_data();
            for ( auto& iter : kfresult->_value )
            {
                ( *pbmail )[ iter.first ] = iter.second;
            }

            // 使用邮件列表的status, 因为有全局邮件
            ( *pbmail )[ __STRING__( status ) ] = mailiter.second;
        }

        if ( ack.mail_size() != 0 )
        {
            _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_MAIL_ACK, &ack );
        }

        // 删除过期邮件
        if ( !overduelist.empty() )
        {
            _mail_driver->Update( overduelist, "hdel {}:{}", __STRING__( maillist ), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddMailReq );

        // 邮件数据
        StringMap maildata;
        auto pbmail = &kfmsg.pbmail().data();
        for ( auto iter = pbmail->begin(); iter != pbmail->end(); ++iter )
        {
            maildata[ iter->first ] = iter->second;
        }

        // 添加邮件
        auto mailid = AddMail( kfmsg.flag(), kfmsg.objectid(), maildata );
        if ( mailid == _invalid_int )
        {
            std::string strdata;
            google::protobuf::util::MessageToJsonString( kfmsg, &strdata );
            __LOG_ERROR__( "objectid[{}] add mail[{}] failed", kfmsg.objectid(), strdata );
        }
    }

    uint64 KFMailShardModule::AddMail( uint32 flag, uint64 objectid, StringMap& maildata )
    {
        // 创建一个邮件id
        auto uint64result = _mail_driver->Execute( "incr {}", __STRING__( mailidcreater ) );
        if ( uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto mailid = uint64result->_value;

        maildata[ __STRING__( id ) ] = __TO_STRING__( mailid );
        maildata[ __STRING__( flag ) ] = __TO_STRING__( flag );
        maildata[ __STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        // 添加邮件
        _mail_driver->Append( maildata, "hmset {}:{}", __STRING__( mail ), mailid );

        auto validtime = maildata[ __STRING__( validtime ) ];
        if ( validtime.empty() )
        {
            validtime = "2592000";	// 默认30天有效期
            maildata[ __STRING__( validtime ) ] = validtime;
        }
        _mail_driver->Append( "expire {}:{} {}", __STRING__( mail ), mailid, validtime );

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
            _mail_driver->Append( "zadd {}:{} {} {}", __STRING__( globalmail ), objectid, mailid, mailid );
            _mail_driver->Append( "sadd {} {}", __STRING__( mailzonelist ), objectid );
            break;
        case KFMsg::PersonMail:	// 个人邮件
            _mail_driver->Append( "hset {}:{} {} {}", __STRING__( maillist ), objectid, mailid, KFMsg::InitStatus );
            break;
        default:
            break;
        }

        // 执行添加
        auto kfresult = _mail_driver->Pipeline();
        if ( kfresult->IsOk() )
        {
            if ( flag == KFMsg::PersonMail )
            {
                // 通知有新邮件
                auto kfresult = _auth_driver->QueryUInt64( "hget {}:{} {}", __STRING__( online ), objectid, __STRING__( game ) );
                if ( kfresult->_value != _invalid_int )
                {
                    KFMsg::S2SNoticeNewMailReq notice;
                    notice.set_playerid( objectid );
                    _kf_route->SendToServer( kfresult->_value, KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &notice, true );
                }
            }
        }
        else
        {
            mailid = _invalid_int;
        }

        return mailid;
    }

    bool KFMailShardModule::UpdateMailStatus( uint32 flag, uint64 playerid, uint64 mailid, uint32 status )
    {
        {
            // 判断玩家是否有拥有此邮件
            auto kfquery = _mail_driver->QueryString( "hget {}:{} {}", __STRING__( maillist ), playerid, mailid );
            if ( !kfquery->IsOk() || kfquery->_value == _invalid_string )
            {
                return false;
            }
        }

        auto ok = false;
        switch ( status )
        {
        case KFMsg::Remove:
        case KFMsg::ReceiveRemove:
        {
            _mail_driver->Append( "hdel {}:{} {}", __STRING__( maillist ), playerid, mailid );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _mail_driver->Append( "del {}:{}", __STRING__( mail ), mailid );
            }

            auto kfresult = _mail_driver->Pipeline();
            ok = kfresult->IsOk();
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            auto kfresult = _mail_driver->Execute( "hset {}:{} {} {}", __STRING__( maillist ), playerid, mailid, status );
            ok = kfresult->IsOk();
        }
        break;
        default:
            break;
        }

        return ok;
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleUpdateMailStatusReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateMailStatusReq );

        auto ok = UpdateMailStatus( kfmsg.flag(), kfmsg.playerid(), kfmsg.id(), kfmsg.status() );
        if ( !ok )
        {
            return;
        }

        KFMsg::S2SUpdateMailStatusAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_id( kfmsg.id() );
        ack.set_status( kfmsg.status() );
        _kf_route->SendToRoute( route, KFMsg::S2S_UPDATE_MAIL_STATUS_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleNewPlayerMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SNewPlayerMailReq );

        // 设置最大的gm邮件id
        {
            auto listresult = _mail_driver->QueryList( "zrevrange {}:{} 0 0", __STRING__( globalmail ), _invalid_int );
            if ( !listresult->_value.empty() )
            {
                auto& strmaxmailid = listresult->_value.front();
                _mail_driver->Execute( "hset {}:{}:{} {} {}", __STRING__( mailinfo ), kfmsg.playerid(), _invalid_int, __STRING__( gmmailid ), strmaxmailid );
            }
        }

        {
            auto listresult = _mail_driver->QueryList( "zrevrange {}:{} 0 0", __STRING__( globalmail ), kfmsg.zoneid() );
            if ( !listresult->_value.empty() )
            {
                auto& strmaxmailid = listresult->_value.front();
                _mail_driver->Execute( "hset {}:{}:{} {} {}", __STRING__( mailinfo ), kfmsg.playerid(), kfmsg.zoneid(), __STRING__( gmmailid ), strmaxmailid );
            }
        }
    }
}