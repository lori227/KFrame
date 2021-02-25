#include "KFMailShardModule.hpp"

namespace KFrame
{
    void KFMailShardModule::BeforeRun()
    {
        auto mail_database_type = KFGlobal::Instance()->GetUInt32( __STRING__( maildatabase ) );
        switch ( mail_database_type )
        {
        case KFDatabaseEnum::Mongo:
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

        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_MAIL_REQ, &KFMailShardModule::HandleQueryMailReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_ADD_MAIL_REQ, &KFMailShardModule::HandleAddMailReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_UPDATE_MAIL_STATUS_REQ, &KFMailShardModule::HandleUpdateMailStatusReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_NEW_PLAYER_MAIL_REQ, &KFMailShardModule::HandleNewPlayerMailReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFMailShardModule::BeforeShut()
    {
        __DELETE_OBJECT__( _mail_database_logic );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_DELAYED_0__();
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
        if ( KFGlobal::Instance()->_app_id->GetWorkId() == 1u )
        {
            KFTimeData time_data;
            time_data._flag = KFTimeEnum::Day;
            time_data._hour = 5;
            __REGISTER_DELAYED_NO_DATA__( &time_data, 0, &KFMailShardModule::OnDelayedClearWholeOverdueMail );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMAddMailReq )
    {
        auto str_data = data;
        KFUtility::ReplaceString( str_data, " ", "" );
        KFUtility::ReplaceString( str_data, "%", "" );

        __JSON_PARSE_STRING__( request, strdata );

        // 邮件玩家列表
        UInt64Set player_list;
        if ( __JSON_HAS_MEMBER__( request, __STRING__( playerid ) ) )
        {
            auto str_player_list = __JSON_GET_STRING__( request, __STRING__( playerid ) );
            KFUtility::SplitSet( str_player_list, ",", player_list );
            __JSON_REMOVE__( request, __STRING__( playerid ) );
        }

        // 小区列表
        UInt64Set zone_list;
        if ( __JSON_HAS_MEMBER__( request, __STRING__( zoneid ) ) )
        {
            auto str_zone_id = __JSON_GET_STRING__( request, __STRING__( zoneid ) );
            KFUtility::SplitSet( str_zone_id, ",", zone_list );
            __JSON_REMOVE__( request, __STRING__( zoneid ) );
        }

        // 邮件内容
        StringMap values;
        __JSON_TO_MAP__( request, values );

        if ( player_list.empty() )
        {
            for ( auto zone_id : zonelist )
            {
                AddMail( KFMsg::GlobalMail, zone_id, values );
            }
        }
        else
        {
            for ( auto player_id : player_list )
            {
                AddMail( KFMsg::PersonMail, player_id, values );
            }
        }

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFMailShardModule::HandleGMDeleteMailReq )
    {
        __JSON_PARSE_STRING__( request, data );

        const auto& mail_list = __JSON_GET_ARRRY__( request, __STRING__( mail_id ) );
        auto count = __JSON_ARRAY_SIZE__( mail_list );

        UInt64List mail_id_list;
        for ( auto i = 0u; i < count; ++i )
        {
            auto mail_id = __JSON_ARRAY_INDEX__( mail_list, i ).GetUint64();
            mail_id_list.push_back( mail_id );
        }
        _mail_database_logic->RemoveMail( mail_id_list );

        return _kf_http_server->SendCode( KFMsg::Ok );
    }

    __KF_DELAYED_FUNCTION__( KFMailShardModule::OnDelayedClearWholeOverdueMail )
    {
        // 清空过期的全局邮件列表
        _mail_database_logic->ClearOverdueGlobalMail();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleQueryMailReq, KFMsg::S2SQueryMailReq )
    {
        // 全局邮件信息
        _mail_database_logic->LoadGlobalMailToPerson( kfmsg->playerid(), _invalid_int );

        // 小区邮件
        _mail_database_logic->LoadGlobalMailToPerson( kfmsg->playerid(), kfmsg->zone_id() );

        // 查询邮件列表
        auto mail_list_result = _mail_database_logic->QueryMailList( kfmsg->playerid(), kfmsg->maxid() );
        if ( mail_list_result == nullptr || mail_list_result->_value.empty() )
        {
            return;
        }

        KFMsg::S2SQueryMailAck ack;
        ack.set_playerid( kfmsg->playerid() );
        for ( auto& mail_data : mail_list_result->_value )
        {
            auto& pb_mail = *( ack.add_mail()->mutable_data() );
            __MAP_TO_PROTO__( mail_data, pb_mail );
        }
        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_MAIL_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleAddMailReq, KFMsg::S2SAddMailReq )
    {
        // 邮件数据
        StringMap m;
        auto pb_mail = &kfmsg->pb_mail().data();
        __PROTO_TO_MAP__( pb_mail, mail_data );

        // 添加邮件
        auto mail_id = AddMail( kfmsg->flag(), kfmsg->objectid(), mail_data );
        if ( mail_id == _invalid_int )
        {
            std::string str_data;
            google::protobuf::util::MessageToJsonString( *kfmsg, &str_data );
            __LOG_ERROR__( "objectid=[{}] add mail=[{}] failed", kfmsg->objectid(), str_data );
        }
    }

    uint64 KFMailShardModule::AddMail( uint32 flag, uint64 object_id, StringMap& mail_data )
    {
        mail_data[ __STRING__( flag ) ] = __TO_STRING__( flag );
        mail_data[ __STRING__( sendtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        auto valid_time = __TO_UINT32__( mail_data[ __STRING__( validtime ) ] );
        if ( valid_time == 0u )
        {
            valid_time = 2592000;	// 默认30天有效期
            mail_data[ __STRING__( validtime ) ] = __TO_STRING__( valid_time );
        }

        auto mail_id = _mail_database_logic->AddMail( flag, object_id, mail_data, valid_time );
        if ( mail_id != 0u && flag == KFMsg::PersonMail )
        {
            // 通知玩家有新邮件
            auto server_id = _kf_basic_database->QueryBasicServerId( object_id );
            if ( server_id != _invalid_int )
            {
                KFMsg::S2SNoticeNewMailReq notice;
                notice.set_playerid( object_id );
                _kf_route->RepeatToEntity( 0, server_id, object_id, KFMsg::S2S_NOTICE_NEW_MAIL_REQ, &notice );
            }
        }

        return mail_id;
    }


    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleUpdateMailStatusReq, KFMsg::S2SUpdateMailStatusReq )
    {
        auto ok = _mail_database_logic->UpdateMailStatus( kfmsg->flag(), kfmsg->playerid(), kfmsg->id(), kfmsg->status() );
        if ( !ok )
        {
            return;
        }

        KFMsg::S2SUpdateMailStatusAck ack;
        ack.set_id( kfmsg->id() );
        ack.set_status( kfmsg->status() );
        ack.set_playerid( kfmsg->playerid() );
        _kf_route->SendToRoute( route, KFMsg::S2S_UPDATE_MAIL_STATUS_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFMailShardModule::HandleNewPlayerMailReq, KFMsg::S2SNewPlayerMailReq )
    {
        // 设置最大的gm邮件id
        _mail_database_logic->InitNewPlayerMail( kfmsg->playerid(), kfmsg->zoneid() );
    }
}