#include "KFMailShardModule.h"

namespace KFrame
{
    void KFMailShardModule::BeforeRun()
    {
        __REGISTER_ROUTE_CONNECTION_FUNCTION__( &KFMailShardModule::OnConnectRoute );

        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ), true, &KFMailShardModule::HandleGMAddMailReq );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( delmail ), true, &KFMailShardModule::HandleGMDeleteMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_REQ, &KFMailShardModule::HandleUpdateMailStatusReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_NEW_PLAYER_MAIL_REQ, &KFMailShardModule::HandleNewPlayerMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailShardModule::BeforeShut()
    {
        __UNREGISTER_SCHEDULE_FUNCTION__();
        __UNREGISTER_ROUTE_CONNECTION_FUNCTION__();
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( addmail ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( delmail ) );

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_MAIL_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ADD_MAIL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MAIL_STATUS_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_NEW_PLAYER_MAIL_REQ );
    }

    void KFMailShardModule::OnceRun()
    {
        // 初始化redis
        _mail_driver = _kf_redis->Create( __KF_STRING__( mail ) );

        // 每天5点 清理过期的全局邮件
        auto kfsetting = _kf_schedule->CreateScheduleSetting();
        kfsetting->SetDate( KFScheduleEnum::Loop, 0, 5 );
        __REGISTER_SCHEDULE_FUNCTION__( kfsetting, &KFMailShardModule::OnScheduleClearWholeOverdueMail );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_ROUTE_CONNECTION_FUNCTION__( KFMailShardModule::OnConnectRoute )
    {
        RouteObjectList objectlist;
        _kf_route->SyncObject( __KF_STRING__( mail ), objectlist );
    }

    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMAddMailReq )
    {
        auto strdata = data;
        KFUtility::ReplaceString( strdata, " ", "" );
        KFUtility::ReplaceString( strdata, "%", "" );

        __JSON_PARSE_STRING__( request, strdata );

        // 邮件玩家列表
        std::set< uint64 > playerlist;
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( playerid ) ) )
        {
            auto strplayerid = __JSON_GET_STRING__( request, __KF_STRING__( playerid ) );
            while ( !strplayerid.empty() )
            {
                auto playerid = KFUtility::SplitValue( strplayerid, "," );
                if ( playerid != _invalid_int )
                {
                    playerlist.insert( playerid );
                }
            }
            __JOSN_REMOVE__( request, __KF_STRING__( playerid ) );
        }

        // 小区列表
        std::set< uint64 > zonelist;
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( zoneid ) ) )
        {
            auto strzoneid = __JSON_GET_STRING__( request, __KF_STRING__( zoneid ) );
            while ( !strzoneid.empty() )
            {
                auto zoneid = KFUtility::SplitValue( strzoneid, "," );
                if ( zoneid != _invalid_int )
                {
                    zonelist.insert( zoneid );
                }
            }
            __JOSN_REMOVE__( request, __KF_STRING__( zoneid ) );
        }

        // 邮件内容
        MapString values;
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

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMDeleteMailReq )
    {
        __JSON_PARSE_STRING__( request, data );

        auto& maillist = __JSON_GET_ARRRY__( request, __KF_STRING__( mailid ) );
        auto count = __JSON_ARRAY_SIZE__( maillist );
        for ( auto i = 0u; i < count; ++i )
        {
            auto mailid = __JSON_ARRAY_INDEX__( maillist, i ).GetUint64();
            _mail_driver->Append( "del {}:{}", __KF_STRING__( mail ), mailid );
        }
        _mail_driver->Pipeline();

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    __KF_SCHEDULE_FUNCTION__( KFMailShardModule::OnScheduleClearWholeOverdueMail )
    {
        // 查询列表
        auto kflist = _mail_driver->QueryList( "smembers {}", __KF_STRING__( mailzonelist ) );
        if ( kflist->_value.empty() )
        {
            return;
        }

        // 清空过期的全局邮件列表
        for ( auto zoneid : kflist->_value )
        {
            auto globalmailkey = __FORMAT__( "{}:{}", __KF_STRING__( globalmail ), zoneid );

            auto kfresult = _mail_driver->QueryList( "zrangebyscore {} -inf +inf", globalmailkey );
            if ( kfresult->_value.empty() )
            {
                continue;
            }

            ListString overduelist;
            for ( auto& strmailid : kfresult->_value )
            {
                auto stringresult = _mail_driver->QueryString( "hget {}:{} {}", __KF_STRING__( mail ), strmailid, __KF_STRING__( id ) );
                if ( stringresult->IsOk() && stringresult->_value == _invalid_str )
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
        auto mailinfokey = __FORMAT__( "{}:{}:{}", __KF_STRING__( mailinfo ), playerid, zoneid );

        // 获取玩家已经加载的最近一封GM邮件id
        auto kfmailid = _mail_driver->QueryUInt64( "hget {} {}", mailinfokey, __KF_STRING__( gmmailid ) );
        if ( !kfmailid->IsOk() )
        {
            return;
        }

        // 查询全局邮件列表
        auto listresult = _mail_driver->QueryList( "zrangebyscore {}:{} ({} +inf", __KF_STRING__( globalmail ), zoneid, kfmailid->_value );
        if ( listresult->_value.empty() )
        {
            return;
        }

        auto& newmaxmailid = listresult->_value.back();
        _mail_driver->Append( "hset {} {} {}", mailinfokey, __KF_STRING__( gmmailid ), newmaxmailid );

        // 添加到个人邮件列表中
        for ( auto& strmailid : listresult->_value )
        {
            _mail_driver->Append( "hset {}:{} {} {}", __KF_STRING__( maillist ), playerid, strmailid, KFMsg::InitStatus );
        }

        auto kfresult = _mail_driver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            __LOG_ERROR__( "reload gm mail[{}] failed!", playerid );
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
        auto mailkeylist = _mail_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( maillist ), kfmsg.playerid() );
        if ( mailkeylist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg.playerid() );

        ListString overduelist;
        for ( auto& mailiter : mailkeylist->_value )
        {
            auto& strmailid = mailiter.first;

            // 判断是否已经下发的邮件
            auto mailid = KFUtility::ToValue<uint64>( strmailid );
            if ( mailid <= kfmsg.maxid() )
            {
                continue;
            }

            auto kfresult = _mail_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( mail ), strmailid );
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
            ( *pbmail )[ __KF_STRING__( status ) ] = mailiter.second;
        }

        if ( ack.mail_size() != 0 )
        {
            _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_MAIL_ACK, &ack );
        }

        // 删除过期邮件
        if ( !overduelist.empty() )
        {
            _mail_driver->Update( overduelist, "hdel {}:{}", __KF_STRING__( maillist ), kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddMailReq );

        // 邮件数据
        MapString maildata;
        auto pbmail = &kfmsg.pbmail().data();
        for ( auto iter = pbmail->begin(); iter != pbmail->end(); ++iter )
        {
            maildata[ iter->first ] = iter->second;
        }

        // 添加邮件
        auto mailid = AddMail( kfmsg.flag(), kfmsg.objectid(), maildata );
        if ( mailid != _invalid_int )
        {
            if ( kfmsg.flag() == KFMsg::PersonMail )
            {
                // 通知有新邮件
                KFMsg::S2SNoticeNewMailReq notice;
                notice.set_playerid( kfmsg.objectid() );
                _kf_route->SendToObject( kfmsg.objectid(), __KF_STRING__( player ), kfmsg.objectid(), KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &notice );
            }
        }
        else
        {
            auto strmaildata = kfmsg.DebugString();
            __LOG_ERROR__( "objectid[{}] add mail[{}] failed!", kfmsg.objectid(), strmaildata );
        }
    }

    uint64 KFMailShardModule::AddMail( uint32 flag, uint64 objectid, MapString& maildata )
    {
        // 创建一个邮件id
        auto uint64result = _mail_driver->Execute( "incr {}", __KF_STRING__( mailidcreater ) );
        if ( uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto mailid = uint64result->_value;

        maildata[ __KF_STRING__( id ) ] = __TO_STRING__( mailid );
        maildata[ __KF_STRING__( flag ) ] = __TO_STRING__( flag );
        maildata[ __KF_STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );

        // 添加邮件
        _mail_driver->Append( maildata, "hmset {}:{}", __KF_STRING__( mail ), mailid );

        auto validtime = maildata[ __KF_STRING__( validtime ) ];
        if ( validtime.empty() )
        {
            validtime = "2592000";	// 默认30天有效期
            maildata[ __KF_STRING__( validtime ) ] = validtime;
        }
        _mail_driver->Append( "expire {}:{} {}", __KF_STRING__( mail ), mailid, validtime );

        switch ( flag )
        {
        case KFMsg::GlobalMail: // 全局邮件
            _mail_driver->Append( "zadd {}:{} {} {}", __KF_STRING__( globalmail ), objectid, mailid, mailid );
            _mail_driver->Append( "sadd {} {}", __KF_STRING__( mailzonelist ), objectid );
            break;
        case KFMsg::PersonMail:	// 个人邮件
            _mail_driver->Append( "hset {}:{} {} {}", __KF_STRING__( maillist ), objectid, mailid, KFMsg::InitStatus );
            break;
        default:
            break;
        }

        // 执行添加
        auto kfresult = _mail_driver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            mailid = _invalid_int;
        }

        return mailid;
    }

    bool KFMailShardModule::UpdateMailStatus( uint32 flag, uint64 playerid, uint64 mailid, uint32 status )
    {
        {
            // 判断玩家是否有拥有此邮件
            auto kfquery = _mail_driver->QueryString( "hget {}:{} {}", __KF_STRING__( maillist ), playerid, mailid );
            if ( !kfquery->IsOk() || kfquery->_value == _invalid_str )
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
            _mail_driver->Append( "hdel {}:{} {}", __KF_STRING__( maillist ), playerid, mailid );
            if ( flag == KFMsg::PersonMail )
            {
                // 删除个人邮件数据
                _mail_driver->Append( "del {}:{}", __KF_STRING__( mail ), mailid );
            }

            auto kfresult = _mail_driver->Pipeline();
            ok = kfresult->IsOk();
        }
        break;
        case KFMsg::DoneStatus:
        case KFMsg::ReceiveStatus:
        {
            ok = _mail_driver->Execute( "hset {}:{} {} {}", __KF_STRING__( maillist ), playerid, mailid, status );
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
            auto listresult = _mail_driver->QueryList( "zrevrange {}:{} 0 0", __KF_STRING__( globalmail ), _invalid_int );
            if ( !listresult->_value.empty() )
            {
                auto& strmaxmailid = listresult->_value.front();
                _mail_driver->Execute( "hset {}:{}:{} {} {}", __KF_STRING__( mailinfo ), kfmsg.playerid(), _invalid_int, __KF_STRING__( gmmailid ), strmaxmailid );
            }
        }

        {
            auto listresult = _mail_driver->QueryList( "zrevrange {}:{} 0 0", __KF_STRING__( globalmail ), kfmsg.zoneid() );
            if ( !listresult->_value.empty() )
            {
                auto& strmaxmailid = listresult->_value.front();
                _mail_driver->Execute( "hset {}:{}:{} {} {}", __KF_STRING__( mailinfo ), kfmsg.playerid(), kfmsg.zoneid(), __KF_STRING__( gmmailid ), strmaxmailid );
            }
        }
    }
}