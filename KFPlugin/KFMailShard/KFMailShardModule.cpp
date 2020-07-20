#include "KFMailShardModule.hpp"

namespace KFrame
{
    void KFMailShardModule::BeforeRun()
    {
        auto maildatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( maildatabase ) );
        switch ( maildatabasetype )
        {
        case KFMsg::Mongo:
            _mail_database_logic = __NEW_OBJECT__( KFMailDatabaseMongo );
            break;
        default:
            _mail_database_logic = __NEW_OBJECT__( KFMailDatabaseRedis );
            break;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        __DELETE_OBJECT__( _mail_database_logic );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        UInt64List mailidlist;
        for ( auto i = 0u; i < count; ++i )
        {
            auto mailid = __JSON_ARRAY_INDEX__( maillist, i ).GetUint64();
            mailidlist.push_back( mailid );
        }
        _mail_database_logic->RemoveMail( mailidlist );

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_SCHEDULE_FUNCTION__( KFMailShardModule::OnScheduleClearWholeOverdueMail )
    {
        // 清空过期的全局邮件列表
        _mail_database_logic->ClearOverdueGlobalMail();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleQueryMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryMailReq );

        // 全局邮件信息
        _mail_database_logic->LoadGlobalMailToPerson( kfmsg.playerid(), _invalid_int );

        // 小区邮件
        _mail_database_logic->LoadGlobalMailToPerson( kfmsg.playerid(), kfmsg.zoneid() );

        // 查询邮件列表
        auto kfmailist = _mail_database_logic->QueryMailList( kfmsg.playerid(), kfmsg.maxid() );
        if ( kfmailist == nullptr || kfmailist->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg.playerid() );
        for ( auto& maildata : kfmailist->_value )
        {
            auto& pbmail = *( ack.add_mail()->mutable_data() );
            __MAP_TO_PROTO__( maildata, pbmail );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_MAIL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddMailReq );

        // 邮件数据
        StringMap maildata;
        auto pbmail = &kfmsg.pbmail().data();
        __PROTO_TO_MAP__( pbmail, maildata );

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
        maildata[ __STRING__( flag ) ] = __TO_STRING__( flag );
        maildata[ __STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        auto validtime = __TO_UINT32__( maildata[ __STRING__( validtime ) ] );
        if ( validtime == 0u )
        {
            validtime = 2592000;	// 默认30天有效期
            maildata[ __STRING__( validtime ) ] = __TO_STRING__( validtime );
        }

        auto mailid = _mail_database_logic->AddMail( flag, objectid, maildata, validtime );
        if ( mailid != 0u && flag == KFMsg::PersonMail )
        {
            // 通知玩家有新邮件
            auto serverid = _kf_basic_database->QueryBasicServerId( objectid );
            if ( serverid != _invalid_int )
            {
                KFMsg::S2SNoticeNewMailReq notice;
                notice.set_playerid( objectid );
                _kf_route->RepeatToServer( serverid, KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &notice );
            }
        }

        return mailid;
    }


    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleUpdateMailStatusReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateMailStatusReq );

        auto ok = _mail_database_logic->UpdateMailStatus( kfmsg.flag(), kfmsg.playerid(), kfmsg.id(), kfmsg.status() );
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
        _mail_database_logic->InitNewPlayerMail( kfmsg.playerid(), kfmsg.zoneid() );
    }
}