#include "KFGuildShardModule.h"
//#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __USE_WORKER__
#define __GUILD_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( guild ) )
#define __PUBLIC_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( public ) )

#ifdef __USE_WORKER__
    #define __REGISTER_GUILD_MESSAGE__ __REGISTER_WORKER_MESSAGE__
    #define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendToClient( kfid, msgid, message )
#else
    #define __REGISTER_GUILD_MESSAGE__ __REGISTER_MESSAGE__
    #define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_MESSAGE__
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendToClient( kfid, msgid, message )
#endif

#define __REDIS_PIPE_LIMIT_BEGIN__  auto cursor = _invalid_int
#define __REDIS_PIPE_PROCESS__	if ( _invalid_int == cursor % 10 && _invalid_int != cursor )\
    {\
        redisdriver->Pipeline();\
    }\
    ++cursor

#define __REDIS_PIPE_LIMIT_END__   if ( cursor > _invalid_int )\
    {\
        redisdriver->Pipeline();\
    }


    KFGuildShardModule::KFGuildShardModule()
    {

    }

    KFGuildShardModule::~KFGuildShardModule()
    {

    }

    void KFGuildShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_guild_shard_config, true );
    }

    void KFGuildShardModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( guild ) );

        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_TO_SHARD_REQ, &KFGuildShardModule::HandleCreateGuildToShard );
        __REGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_REQ, &KFGuildShardModule::HandleInviteGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_REQ, &KFGuildShardModule::HandleApplyGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_EXIT_GUILD_REQ, &KFGuildShardModule::HandleExitGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSFER_MASTER_REQ, &KFGuildShardModule::HandleTransferMasterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MEMBER_BASIC_DATA_REQ, &KFGuildShardModule::HandleUpdateMemberBasic );
        __REGISTER_MESSAGE__( KFMsg::S2S_REVIEW_APPLY_REQ, &KFGuildShardModule::HandleReviewApplyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_DISSOLVE_GUILD_REQ, &KFGuildShardModule::HandleDissolveGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_MODIFY_MEDAL_REQ, &KFGuildShardModule::HandleModifyMedalReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LIST_REQ, &KFGuildShardModule::HnadleQueryGuildListReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_MEMBER_REQ, &KFGuildShardModule::HnadleKickMemberReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILD_REQ, &KFGuildShardModule::HandleQueryGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ, &KFGuildShardModule::HandleSearchGuildByNameReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SET_GUILD_SWITCH_REQ, &KFGuildShardModule::HandleSetGuildSwitchReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ, &KFGuildShardModule::HandleLoginQueryGuildIdReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LOG_REQ, &KFGuildShardModule::HandleQueryGuildLogReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ADD_GUILD_ACTIVENESS_REQ, &KFGuildShardModule::HandleAddGuildActivenessReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GUILD_DATA_REQ, &KFGuildShardModule::HandleUpdateGuildDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_UPGRADE_GUILD_REQ, &KFGuildShardModule::HandleUpgradeGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPOINT_GUILD_MEMBER_REQ, &KFGuildShardModule::HandleAppointGuildMemberReq );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFGuildShardModule::OnServerDiscoverClient );

        _kf_component->SetEntityDataMask( __DELETE_AND_REMOVE__ );
        _kf_component->RegisterEntityDeleteFunction( this, &KFGuildShardModule::DeleteGuild );
        _kf_component->RegisterRemoveDataModule( this, &KFGuildShardModule ::OnRemoveDataCallBack );
        _kf_component->RegisterAddDataModule( this, &KFGuildShardModule::OnAddDataCallBack );
        _kf_component->RegisterUpdateDataModule( this, &KFGuildShardModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateStringModule( this, &KFGuildShardModule::OnUpdateStringCallBack );
        // 注册更新函数
        _kf_component->RegisterSyncAddFunction( this, &KFGuildShardModule::SendAddDataToClient );
        _kf_component->RegisterSyncRemoveFunction( this, &KFGuildShardModule::SendRemoveDataToClient );
        _kf_component->RegisterSyncUpdateFunction( this, &KFGuildShardModule::SendUpdateDataToClient );

    }

    void KFGuildShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MEMBER_BASIC_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_EXIT_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSFER_MASTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REVIEW_APPLY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_DISSOLVE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_MODIFY_MEDAL_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LIST_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_MEMBER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SEARCH_GUILD_BY_NAME_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_QUERY_GUILDID_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_GUILD_LOG_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_GUILD_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPGRADE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPOINT_GUILD_MEMBER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_SET_GUILD_SWITCH_REQ );

        _kf_component->UnRegisterEntityDeleteFunction();
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );


        // 卸载更新函数
        _kf_component->UnRegisterSyncAddFunction();
        _kf_component->UnRegisterSyncRemoveFunction();
        _kf_component->UnRegisterSyncUpdateFunction();

    }

    void KFGuildShardModule::OnceRun()
    {
        LoadGuildData();

        __REGISTER_LOOP_TIMER__( KFGlobal::Instance()->_app_id._union._id, KFTimeEnum::OneMinuteMicSecond * 10, &KFGuildShardModule::OnTimerRefreshGuild );
    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFGuildShardModule::OnServerDiscoverClient )
    {
        if ( handlename == __KF_STRING__( guild ) && handletype == __KF_STRING__( proxy ) )
        {
            std::set< uint64 > guildlist;

            auto* kfguild = _kf_component->FirstEntity();
            while ( kfguild != nullptr )
            {
                guildlist.insert( kfguild->GetKeyID() );
                kfguild = _kf_component->NextEntity();
            }

            _kf_cluster_shard->AddObjectToProxy( handleid, guildlist );
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    void KFGuildShardModule::DeleteGuild( KFEntity* kfentity )
    {
        if ( !kfentity->IsInited() )
        {
            return;
        }
        auto kfobject = kfentity->GetData();
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto guildid = kfobject->GetKeyID();
        auto guildname = kfobject->GetValue< std::string >( __KF_STRING__( name ) );

        // 删除guild列表
        redisdriver->Append( "del {}:{}", __KF_STRING__( guild ), guildid );
        // 删除帮派名字和id对应的hash
        redisdriver->Append( "hdel {} {}", __KF_STRING__( guildnameidhash ), guildname );
        // 移除本shard管理的guildid集合
        redisdriver->Append( "srem {}:{} {}", __KF_STRING__( guildidset ), KFGlobal::Instance()->_app_id._union._id, guildid );
        // 删除帮派的排行榜
        redisdriver->Append( "zrem {} {}", __KF_STRING__( guildrank ), guildid );
        // 删除帮派日志
        redisdriver->Append( "del {}:{}", __KF_STRING__( guildlog ), guildid );

        redisdriver->Pipeline( );

        //  remove proxy handle
        _kf_cluster_shard->RemoveObjectToProxy( guildid );
        // 这边处理帮派玩家列表 // 发送到client 通知在线玩家帮派解散，操作离线玩家public guild字段,是否添加邮件?
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );

        auto kfguildmember = kfguildmembers->FirstData();

        __REDIS_PIPE_LIMIT_BEGIN__;
        while ( kfguildmember != nullptr )
        {
            if ( _invalid_int == kfguildmember->GetKeyID() )
            {
                kfguildmember = kfguildmembers->NextData();
                __LOG_ERROR__( "del guild[{}] member id is empty", guildid );
                continue;
            }

            __REDIS_PIPE_PROCESS__;

            redisdriver->Append( "del {}:{}", __KF_STRING__( guildmember ), kfguildmember->GetKeyID() );

            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            auto playerid = kfguildmember->GetKeyID();

            if ( _invalid_int != serverid )
            {
                if ( SendPlayerGuildChangeToClient( KFId( _invalid_int, serverid ), playerid, guildid, KFMsg::ExitGuild ) )
                {
                    kfguildmember = kfguildmembers->NextData();
                    continue;
                }

            }
            RemovePlayerGuildId( playerid );
            kfguildmember = kfguildmembers->NextData();

        }

        __REDIS_PIPE_LIMIT_END__

    }

    __KF_REMOVE_DATA_FUNCTION__( KFGuildShardModule::OnRemoveDataCallBack )
    {
        if ( !player->IsInited() )
        {
            return;
        }

        // 删除移除的子属性的key
        if ( kfdata->GetName().empty() ||  _invalid_int == key )
        {
            return;
        }

        if ( kfparent->HaveFlagMask( KFDataDefine::Mask_Save_Database )
                && kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            auto redisdriver = __GUILD_REDIS_DRIVER__;
            auto result = redisdriver->Execute( "del {}:{}", kfdata->GetName(), key );
            if ( !result->IsOk() )
            {
                __LOG_ERROR__( "guild[{}] remove data failed! field[{}] key[{}]", player->GetKeyID(), kfdata->GetName(), key );
            }
        }

    }

    __KF_UPDATE_DATA_FUNCTION__( KFGuildShardModule::OnUpdateDataCallBack )
    {
        if ( !player->IsInited() )
        {
            return;
        }
        if ( _invalid_int == key )
        {
            auto kfpraent = kfdata->GetParent();
            if ( nullptr != kfpraent )
            {
                key = kfpraent->GetKeyID();
            }
        }

        if ( _invalid_int == key )
        {
            __LOG_ERROR__( "guild update data callback failed! data[{}] newvalue[{}] oldvalue[{}]",
                           kfdata->GetName(), newvalue, oldvalue );
            return;
        }

        auto redisdriver = __GUILD_REDIS_DRIVER__;
        if ( kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) &&
                kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {

            auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
                                                  kfdata->GetParent()->GetName(), key, kfdata->GetName(), newvalue );
            if ( !kfresult->IsOk() )
            {
                __LOG_ERROR__( "guild[{}] update data failed! parentname[{}] key[{}] kfdataname[{}] value[{}]",
                               player->GetKeyID(), kfdata->GetParent()->GetName(), key, kfdata->GetName(), newvalue );
            }
        }

        auto showdata = _kf_guild_shard_config->_show_data;
        if ( !showdata.empty() )
        {
            if ( showdata.end() != std::find( showdata.begin(), showdata.end(), kfdata->GetName() ) )
            {
                auto score = CalcGuildRankDataScore( kfdata->GetParent() );
                if ( _invalid_int < score )
                {
                    auto kfresult = redisdriver->Execute( "zadd {}  {} {}", __KF_STRING__( guildrank ), score, kfdata->GetParent()->GetKeyID() );
                    if ( !kfresult->IsOk() )
                    {
                        __LOG_ERROR__( "guild[{}] update guildrank failed! score[{}] key[{}]",
                                       score, kfdata->GetParent()->GetKeyID() );
                    }
                }
            }
        }
    }

    __KF_ADD_DATA_FUNCTION__( KFGuildShardModule::OnAddDataCallBack )
    {
        if ( !player->IsInited() )
        {
            return;
        }

        if ( kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) &&
                kfparent->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            auto redisdriver = __GUILD_REDIS_DRIVER__;
            auto child = kfdata->FirstData();
            //
            __REDIS_PIPE_LIMIT_BEGIN__;
            while ( child != nullptr )
            {
                if ( !child->HaveFlagMask( KFDataDefine::Mask_Save_Database )
                        || child->GetType() == KFDataDefine::Type_Object
                        || KFDataDefine::Type_Record == child->GetType() )
                {
                    child = kfdata->NextData();
                    continue;
                }

                if ( child->GetName().empty() )
                {
                    child = kfdata->NextData();
                    continue;
                }

                __REDIS_PIPE_PROCESS__;

                redisdriver->Append( "hset {}:{} {} {}", kfdata->GetName(), kfdata->GetKeyID(), child->GetName(), child->GetValue() );

                child = kfdata->NextData();
            }

            __REDIS_PIPE_LIMIT_END__
        }
    }

    __KF_UPDATE_STRING_FUNCTION__( KFGuildShardModule::OnUpdateStringCallBack )
    {
        if ( !player->IsInited() )
        {
            return;
        }

        auto redisdriver = __GUILD_REDIS_DRIVER__;
        if ( kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) &&
                kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            if ( _invalid_str != kfdata->GetValue<std::string>() )
            {
                auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
                                                      kfdata->GetParent()->GetName(), kfdata->GetParent()->GetKeyID(), kfdata->GetName(), kfdata->GetValue<std::string>() );
                if ( !kfresult->IsOk() )
                {
                    __LOG_ERROR__( "guild[{}] update data failed! parentname[{}] parentkey[{}] dataname[{}] data[{}]",
                                   player->GetKeyID(), kfdata->GetParent()->GetName(), kfdata->GetParent()->GetKeyID(), kfdata->GetName(), kfdata->GetValue<std::string>() );
                }
            }
            else
            {
                // 需要特殊处理 _invalid_str
                auto kfresult = redisdriver->Execute( "hdel {}:{} {}",
                                                      kfdata->GetParent()->GetName(), kfdata->GetParent()->GetKeyID(), kfdata->GetName() );
                if ( !kfresult->IsOk() )
                {
                    __LOG_ERROR__( "guild[{}] update data failed! parentname[{}] parentkey[{}] dataname[{}]",
                                   player->GetKeyID(), kfdata->GetParent()->GetName(), kfdata->GetParent()->GetKeyID(), kfdata->GetName() );
                }

            }

        }

        // 帮派排行榜
        auto showdata = _kf_guild_shard_config->_show_data;
        if ( !showdata.empty() )
        {
            if ( showdata.end() != std::find( showdata.begin(), showdata.end(), kfdata->GetName() ) )
            {
                auto score = CalcGuildRankDataScore( kfdata->GetParent() );
                if ( _invalid_int < score )
                {
                    auto kfresult = redisdriver->Execute( "zadd {}  {} {}", __KF_STRING__( guildrank ), score, kfdata->GetParent()->GetKeyID() );
                    if ( !kfresult->IsOk() )
                    {
                        __LOG_ERROR__( "guild[{}] update guildrank failed! score[{}] key[{}]",
                                       score, kfdata->GetParent()->GetKeyID() );
                    }
                }
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleCreateGuildToShard )
    {
        __PROTO_PARSE__( KFMsg::S2SCreateGuildToShardReq );

        // 创建帮派 判断名字是否重复
        KFMsg::S2SCreateGuildAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_guildid( kfmsg.guildid() );
        ack.set_medal( kfmsg.medal() );
        ack.set_manifesto( kfmsg.manifesto() );

        //// 已经拥有帮派
        //if ( IsGuildMember( kfmsg.playerid() ) )
        //{
        //    ack.set_code( KFMsg::GuildHadBuild );
        //    _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
        //    return;
        //}

        //if ( IsRepeateName( kfmsg.guildname() ) )
        //{
        //    // 名字重复
        //    ack.set_code( KFMsg::GuildNameRepeat );
        //    _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
        //    return;
        //}


        // 记录帮派名字和id对应的hash， field->guildname, value->guildid
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        redisdriver->Append( "hset {} {} {}", __KF_STRING__( guildnameidhash ), kfmsg.guildname(), kfmsg.guildid() );
        // shardid->guildid
        auto kfglobal = KFGlobal::Instance();
        redisdriver->Append( "sadd {}:{} {}", __KF_STRING__( guildidset ), kfglobal->_app_id._union._id, kfmsg.guildid() );

        // 创建帮派实体
        auto kfguild = _kf_component->CreateEntity( kfmsg.guildid() );
        auto kfobject = kfguild->GetData();
        auto strplayerid = KFUtility::ToString( kfmsg.playerid() );

        // 记录帮派详细信息
        MapString values;
        values[ __KF_STRING__( createtime ) ] = KFUtility::ToString( kfglobal->_real_time );
        values[ __KF_STRING__( medal )] = KFUtility::ToString( kfmsg.medal() );
        values[ __KF_STRING__( name ) ] = kfmsg.guildname();
        values[ __KF_STRING__( creator ) ] = kfmsg.name();
        values[ __KF_STRING__( masterid ) ] = strplayerid;
        values[__KF_STRING__( level )] = "1";
        //values[ __KF_STRING__( memberlist ) ] = strplayerid + DEFAULT_SPLIT_STRING;
        values[ __KF_STRING__( totalactiveness ) ] = KFUtility::ToString( _invalid_int );
        values[ __KF_STRING__( manifesto ) ] = kfmsg.manifesto();
        auto strguildid = KFUtility::ToString( kfmsg.guildid() );
        values[ __KF_STRING__( id ) ] = strguildid;
        for ( auto iter : values )
        {
            kfobject->SetValue< std::string >( iter.first, iter.second );
        }
        redisdriver->Append( values, "hmset {}:{}", __KF_STRING__( guild ), kfmsg.guildid() );

        //添加帮派排行榜
        redisdriver->Append( "zadd {} {} {}", __KF_STRING__( guildrank ), _invalid_int, kfmsg.guildid() );

        // 这边设置完初始化标志后 会根据updata kfdata 回调去hset对应的键值对
        kfguild->SetInited();
        auto retcode = JoinGuild( kfmsg.guildid(), kfmsg.playerid(), KFGuildShardEnum::GuildMaster );
        if ( KFMsg::Ok != retcode )
        {
            ack.set_code( retcode );
            _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
        }
        redisdriver->Pipeline();

        // 添加到代理
        std::set< uint64 > objectlist;
        objectlist.insert( kfmsg.guildid() );
        _kf_cluster_shard->AddObjectToProxy( __KF_HEAD_ID__( kfid ), objectlist );

        // 通知client 创建军团成功
        ack.set_code( KFMsg::Ok );
        _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
        // 通知client
        KFMsg::S2SLoginQueryGuildAck guilddataack;
        guilddataack.set_showapplicant( true );
        _kf_kernel->SerializeToOnline( kfguild->GetData(), guilddataack.mutable_guilddata() );
        guilddataack.set_playerid( kfmsg.playerid() );
        guilddataack.set_guildid( kfmsg.guildid() );
        _kf_cluster_shard->SendToClient( __KF_HEAD_ID__( kfid ), kfmsg.serverid(), KFMsg::S2S_LOGIN_QUERY_GUILD_ACK, &guilddataack );

        // 写日志
        auto kfguildsetting = _kf_guild_shard_config->FindGuildSetting( 1 );
        if ( kfguildsetting == nullptr )
        {
            __LOG_ERROR__( "create guild[{}] get setting failed!", kfmsg.guildid() );
            return;
        }
        WriteGuildLog( kfmsg.guildid(), kfguildsetting->_max_log, KFGuildShardEnum::CreateGuild, kfmsg.name() );

    }


    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleUpdateMemberBasic )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateMemberBasicDataReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto kfmemberrecord = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.playerid() );
        // 没有该玩家
        if ( nullptr == kfmemberrecord )
        {
            return;
        }

        auto kfbsics = kfmemberrecord->FindData( __KF_STRING__( basic ) );
        for ( auto i = 0; i < kfmsg.pbdata_size(); ++i )
        {
            auto pbstring = &kfmsg.pbdata( i );
            kfbsics->SetValue<std::string>( pbstring->name(), pbstring->value() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleInviteGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SInviteGuildReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return;
        }
        // 已经是帮派成员
        if ( IsGuildMember( kfmsg.invitedid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.invitor(), KFMsg::Playerisguildmember );
        }

        if ( IsInApplicanlist( kfmsg.guildid(), kfmsg.invitedid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.invitor(), KFMsg::PlayerInApplicanlist );
        }

        KFMsg::S2SInviteGuildAck ack;
        ack.set_guildid( kfmsg.guildid() );
        ack.set_playerid( kfmsg.invitor() );
        ack.set_invitedid( kfmsg.invitedid() );
        _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_INVITE_GUILD_ACK, &ack );
    }


    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleApplyGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SApplyGuildReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        // 已经是帮派成员
        if ( IsGuildMember( kfmsg.playerid() ) )
        {
            return;
        }

        if ( IsInApplicanlist( kfmsg.guildid(), kfmsg.playerid() ) )
        {
            return;
        }

        // 如果是帮主邀请的直接加入帮派
        if ( kfmsg.invitor() == kfobject->GetValue<uint32>( __KF_STRING__( masterid ) ) )
        {
            auto retcode = JoinGuild( kfmsg.guildid(), kfmsg.playerid() );
            if ( KFMsg::Ok == retcode )
            {
                return;
            }
            else
            {
            }

        }
        KFMsg::S2SApplyGuildAck ack;
        ack.set_guildid( kfmsg.guildid() );
        ack.set_playerid( kfmsg.playerid() );
        // 如果是帮主邀请的 但是帮派成员满了就先加到申请列表中
        // 加入申请列表
        if ( !JoinApplicanlist( kfmsg.guildid(), kfmsg.playerid() ) )
        {
            //ack.set_code( KFMsg::GuildApplyListTooLong );
        }
        else
        {
            ack.set_code( KFMsg::Ok );
        }
        _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_APPLY_GUILD_ACK, &ack );
        //_kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_APPLY_GUILD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleExitGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SExitGuildReq );

        if ( !IsInGuild( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            return;
        }

        if ( IsMaster( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GuildMaster );
        }

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        auto kfobject = kfguild->GetData();
        auto strvicemember = kfobject->GetValue <std::string>( __KF_STRING__( vicemaster ) );
        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( KFGuildShardEnum::GuildViceMaster  == title )
        {
            auto delplayerid = KFUtility::ToString( kfmsg.playerid() ) + DEFAULT_SPLIT_STRING;
            std::string newmemberlist = "";
            if ( !KFUtility::DelString( strvicemember, delplayerid, newmemberlist ) )
            {
                __LOG_ERROR__( "player[{}] exit guild[{}] failed! vicmember[{}] delmember[{}]",
                               kfmsg.playerid(), kfmsg.guildid(), strvicemember, delplayerid );
                return;
            }

            kfguild->UpdateData( __KF_STRING__( vicemaster ), newmemberlist );
        }

        // 删除成员列表信息
        if ( !RemoveGuildMember( kfmsg.guildid(), kfmsg.playerid(), false ) )
        {
            return;
        }

        // 发送失败直接操作数据库
        auto ok =  SendPlayerGuildChangeToClient( kfid, kfmsg.playerid(), kfmsg.guildid(), KFMsg::ExitGuild );
        if ( !ok )
        {
            RemovePlayerGuildId( kfmsg.playerid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleTransferMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransferMasterReq );

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        if ( !IsMaster( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        if ( !IsSameGuild( kfmsg.playerid(), kfmsg.newmasterid(), kfmsg.guildid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoSmallGuild );
        }

        // 更新老帮主的职位
        auto kfoldmaster = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.playerid() );
        auto kfnewmaster = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.newmasterid() );
        if ( nullptr == kfoldmaster || nullptr == kfnewmaster )
        {
            return;
        }

        auto title = GetTitle( kfmsg.newmasterid(), kfmsg.guildid() );
        // 副帮主 特殊处理
        if ( KFGuildShardEnum::GuildViceMaster == title )
        {
            auto strvicemember = kfobject->GetValue <std::string>( __KF_STRING__( vicemaster ) );
            auto delplayerid = KFUtility::ToString( kfmsg.newmasterid() ) + DEFAULT_SPLIT_STRING;
            std::string newmemberlist = "";
            if ( !KFUtility::DelString( strvicemember, delplayerid, newmemberlist ) )
            {
                __LOG_ERROR__( "player[{}] transfer guild[{}] failed! vicmember[{}] delmember[{}]",
                               kfmsg.playerid(), kfmsg.guildid(), strvicemember, delplayerid );
                return;
            }

            kfguild->UpdateData( __KF_STRING__( vicemaster ), newmemberlist );
        }


        kfguild->UpdateData( kfoldmaster, __KF_STRING__( title ), KFOperateEnum::Set, KFGuildShardEnum::GuildMember );
        kfguild->UpdateData( kfnewmaster, __KF_STRING__( title ), KFOperateEnum::Set, KFGuildShardEnum::GuildMaster );
        // 设置新的帮主
        kfguild->UpdateData( __KF_STRING__( masterid ), KFOperateEnum::Set, kfmsg.newmasterid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleReviewApplyReq )
    {
        __PROTO_PARSE__( KFMsg::S2SReviewApplyReq );

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }

        // 判断是否有操作权限
        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Review_Data ) )
        {
            // return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        //// 处理单个人
        //if ( kfmsg.has_dealplayerid() )
        //{
        //    if ( !RemoveApplicanlist( kfmsg.guildid(), kfmsg.dealplayerid() ) )
        //    {
        //        return  _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoInApplicanlist );
        //    }

        //    if ( KFMsg::RefuseApply == kfmsg.operatortype() )
        //    {
        //        return;
        //    }
        //    else
        //    {
        //        if ( IsGuildMember( kfmsg.dealplayerid() ) )
        //        {
        //            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::Playerisguildmember );
        //        }

        //        auto retcode = JoinGuild( kfmsg.guildid(), kfmsg.dealplayerid() );
        //        if ( KFMsg::Ok != retcode )
        //        {
        //            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), retcode );
        //        }
        //    }
        //}
        //else
        //{
        //    // 一键拒绝
        //    if ( KFMsg::RefuseApply == kfmsg.operatortype() )
        //    {
        //        auto ok = ClearApplicanlist( kfmsg.guildid() );
        //    }
        //    else
        //    {
        //        // 一键添加
        //        MoveApplicantToGuild( kfmsg.guildid() );

        //    }
        //}
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleDissolveGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SDissolveGuildReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }

        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Dissolve_Data ) )
        {
            // return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        _kf_component->RemoveEntity( kfmsg.guildid() );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleModifyMedalReq )
    {
        __PROTO_PARSE__( KFMsg::S2SModifyMedalReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }

        // 判断是否有操作权限

        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Modify_Data ) )
        {
            // return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        auto kfobject = kfguild->GetData();
        auto oldmedal = kfobject->GetValue<uint32>( __KF_STRING__( medal ) );
        if ( oldmedal == kfmsg.newmedal() )
        {
            //  return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GuildMedalRepeat );
        }

        kfguild->UpdateData( __KF_STRING__( medal ), __TO_STRING__( kfmsg.newmedal() ) );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HnadleQueryGuildListReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryGuildListReq );

        KFMsg::MsgQueryGuildListAck ack;
        // 请求总的帮派列表(从帮派总榜单里读取)
        if ( !kfmsg.has_guilds() )
        {
            QueryGuildListReq( kfmsg.cursor(), &ack );
        }
        else
        {
            // 请求指定帮派列表
            auto pbguilddatas =  ack.mutable_guilddatas();
            auto guilds = kfmsg.guilds();
            for ( auto i = 0 ; i < guilds.guildid_size(); ++i )
            {
                MapString guilddatas;
                if ( !QuerySingleGuildData( guilds.guildid( i ), guilddatas ) )
                {
                    continue;
                }
                SerialGuildData( guilddatas, ack.add_guilddatas() );
            }
        }
        _kf_cluster_shard->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_GUILD_LIST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HnadleKickMemberReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickMemberReq );

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }

        uint32 title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Dismissal_Data ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        if ( !IsGreatTitle( kfmsg.playerid(), kfmsg.toplayerid(), kfmsg.guildid() ) )
        {
            //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        RemoveGuildMember( kfmsg.guildid(), kfmsg.toplayerid() );

        auto serverid = GetServerid( kfmsg.toplayerid() );
        auto testid = GetServerid( kfmsg.playerid() );
        if ( _invalid_int != serverid )
        {
            if ( SendPlayerGuildChangeToClient( KFId( _invalid_int, serverid ), kfmsg.toplayerid(), kfmsg.guildid(), KFMsg::ExitGuild ) )
            {
                return;
            }
        }
        RemovePlayerGuildId( kfmsg.toplayerid() );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleQueryGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryGuildReq );
        if ( !IsInGuild( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            return;
        }
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }

        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        bool isownpower = _kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Review_Data );

        KFMsg::S2SLoginQueryGuildAck ack;
        ack.set_showapplicant( isownpower );
        ack.set_guildid( kfmsg.guildid() );
        _kf_kernel->SerializeToOnline( kfguild->GetData(), ack.mutable_guilddata() );
        ack.set_playerid( kfmsg.playerid() );
        _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_LOGIN_QUERY_GUILD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleSearchGuildByNameReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSearchGuildByNameReq );
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "hget {} {}", __KF_STRING__( guildnameidhash ), kfmsg.guildname() );
        if ( !result->IsOk() || _invalid_int == result->_value )
        {
            return;
        }
        KFMsg::MsgQueryGuildListAck ack;
        MapString guilddatas;
        if ( !QuerySingleGuildData( result->_value, guilddatas ) )
        {
            return;
        }
        SerialGuildData( guilddatas, ack.add_guilddatas() );
        _kf_cluster_shard->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_GUILD_LIST_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleSetGuildSwitchReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSetGuildSwitchReq );
        if ( !IsInGuild( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            return;
        }
        auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Manger_Data ) )
        {
            // return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        kfguild->UpdateData( kfmsg.type(), __TO_STRING__( kfmsg.flag() ) );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleUpdateGuildDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateGuildDataReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto kfmember = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.playerid() );
        if ( nullptr == kfmember )
        {
            return;
        }
        auto kfbasics = kfmember->FindData( __KF_STRING__( basic ) );
        for ( auto i = 0; i < kfmsg.pbdata_size(); ++i )
        {
            auto kfbasic = kfbasics->FindData( kfmsg.pbdata( i ).name() );
            if ( nullptr != kfbasic )
            {
                kfbasic->SetValue<std::string>( kfmsg.pbdata( i ).value() );
            }
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleUpgradeGuildReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpgradeGuildReq );
        KFMsg::S2SUpgradeGuildAck ack;
        ack.set_playerid( kfmsg.playerid() );
        ack.set_level( _invalid_int );

        //auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        //if ( nullptr == kfguild )
        //{
        //    __LOG_ERROR__( "player[{}] updrage guild[{}] level failed!", kfmsg.playerid(), kfmsg.guildid() );
        //    ack.set_code( KFMsg::SysError );
        //}

        //auto kfobject = kfguild->GetData();
        //auto level = kfobject->GetValue<uint32>( __KF_STRING__( level ) );
        //auto kfguildsetting = _kf_guild_shard_config->FindGuildSetting( ++level );
        //ack.set_level( level );
        //if ( nullptr == kfguildsetting )
        //{
        //    ack.set_code( KFMsg::GuildMaxLevel );
        //}

        //if ( !IsInGuild( kfmsg.playerid(), kfmsg.guildid() ) )
        //{
        //    ack.set_code( KFMsg::PlayerNoGuild );
        //}

        //auto title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        //if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Upgrade_Data ) )
        //{
        //    ack.set_code( KFMsg::PlayerNoPower );
        //}

        //// 判断升级活跃度
        //auto totalactiveness = kfobject->GetValue<uint64>( __KF_STRING__( totalactiveness ) );
        //if ( totalactiveness < kfguildsetting->_upgrade_activeness )
        //{
        //    ack.set_code( KFMsg::GuildLackActiveness );
        //}

        //kfguild->UpdateData( __KF_STRING__( level ), KFOperateEnum::Set, level );
        //__LOG_INFO__( "player[{}] upgrade guild[{}] level[{}] success!", kfmsg.guildid(), kfmsg.guildid(), level );
        ack.set_code( KFMsg::Ok );
        _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_UPGRADE_GUILD_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleAppointGuildMemberReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAppointGuildMemberReq );
        uint32 title = GetTitle( kfmsg.playerid(), kfmsg.guildid() );
        if ( !_kf_guild_shard_config->IsOwnPower( title, KFGuildShardEnum::Mask_Appoint_Data ) )
        {
            //  return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        if ( KFGuildShardEnum::GuildMaster == kfmsg.title() )
        {
            return;
        }

        // 是否职位高于被处理者并且是同个帮派
        if ( !IsGreatTitle( kfmsg.playerid(), kfmsg.toplayerid(), kfmsg.guildid() ) )
        {
            // return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::PlayerNoPower );
        }

        auto toplayertitle = GetTitle( kfmsg.toplayerid(), kfmsg.guildid() );
        if ( toplayertitle == kfmsg.title() )
        {
            //  return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GuildMemberOwnTitle );
        }

        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto guildlevel = kfobject->GetValue<uint32>( __KF_STRING__( level ) );
        auto kfguildsetting = _kf_guild_shard_config->FindGuildSetting( guildlevel );
        if ( nullptr == kfguildsetting )
        {
            __LOG_ERROR__( "player[{}] appoint toplayer[{}] guild[{}] title[{}] failed! level[{}] setting is null",
                           kfmsg.playerid(), kfmsg.toplayerid(), kfmsg.guildid(), kfmsg.title(), guildlevel );
        }
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.toplayerid() );
        if ( kfguildmember == nullptr )
        {
            __LOG_ERROR__( "player[{}] appoint toplayer[{}] guild[{}] title[{}] failed! kfguildmember is null",
                           kfmsg.playerid(), kfmsg.toplayerid(), kfmsg.guildid(), kfmsg.title() );
        }
        auto strvicemember = kfobject->GetValue <std::string>( __KF_STRING__( vicemaster ) );
        std::string copyvicemember( strvicemember );
        // 任命的职位是副帮主
        if ( KFGuildShardEnum::GuildViceMaster == kfmsg.title() )
        {
            auto vicemembernum = _invalid_int;
            while ( !copyvicemember.empty() )
            {
                KFUtility::SplitString( copyvicemember, DEFAULT_SPLIT_STRING );
                ++vicemembernum;
            }
            if ( vicemembernum >= kfguildsetting->_max_vicemaster )
            {
                //return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.playerid(), KFMsg::GuildMaxViceMember );
            }
            auto newvicemaster = strvicemember + KFUtility::ToString( kfmsg.toplayerid() ) + DEFAULT_SPLIT_STRING;
            kfguild->UpdateData( __KF_STRING__( vicemaster ), newvicemaster );
        }
        else if ( KFGuildShardEnum::GuildViceMaster == toplayertitle )
        {
            auto delplayerid = KFUtility::ToString( kfmsg.toplayerid() ) + DEFAULT_SPLIT_STRING;
            std::string newmemberlist = "";
            if ( !KFUtility::DelString( strvicemember, delplayerid, newmemberlist ) )
            {
                __LOG_ERROR__( "player[{}] appoint guild[{}] member[{}] failed! memberlist[{}] delplayer[{}]",
                               kfmsg.playerid(), kfmsg.guildid(), kfmsg.toplayerid(), strvicemember, delplayerid );
                return;
            }

            kfguild->UpdateData( __KF_STRING__( vicemaster ), newmemberlist );
        }

        kfguild->UpdateData( kfguildmember, __KF_STRING__( title ), KFOperateEnum::Set,  kfmsg.title() );

    }
    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleLoginQueryGuildIdReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginQueryGuildidReq );

        auto guildid = GetGuildIdByPlayerid( kfmsg.playerid() );
        KFMsg::S2SLoginQueryGuildidAck ack;
        ack.set_guildid( guildid );
        ack.set_playerid( kfmsg.playerid() );
        _kf_cluster_shard->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::S2S_LOGIN_QUERY_GUILDID_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleQueryGuildLogReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryGuildLogReq );
        auto redisdriver = __GUILD_REDIS_DRIVER__;

        auto result = redisdriver->QueryVector( "lrange {}:{} {} {}", __KF_STRING__( guildlog ), kfmsg.guildid(), kfmsg.beginpos(), kfmsg.endpos() );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "player[{}] query guild[{}] failed! page[{}] beginpos[{}] endpos[{}]",
                           kfmsg.playerid(), kfmsg.guildid(), kfmsg.page(), kfmsg.beginpos(), kfmsg.endpos() );
            return;
        }
        if ( result->_value.empty() )
        {
            return;
        }
        KFMsg::MsgQueryGuildLogAck ack;
        ack.set_page( kfmsg.page() );
        for ( auto& iter : result->_value )
        {
            ack.add_guildlog( iter );
        }

        _kf_cluster_shard->SendToPlayer( kfmsg.serverid(), kfmsg.playerid(), KFMsg::MSG_QUERY_GUILD_LOG_ACK, &ack );

    }

    __KF_MESSAGE_FUNCTION__( KFGuildShardModule::HandleAddGuildActivenessReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAddGuildActivenessReq );
        auto kfguild = _kf_component->FindEntity( kfmsg.guildid(), __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            __LOG_ERROR__( "player[{}] add guild[{}] activeness failed!", kfmsg.playerid(), kfmsg.guildid() );
            return;
        }
        if ( !IsInGuild( kfmsg.playerid(), kfmsg.guildid() ) )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.playerid() );
        // 先主动刷新个人的周活跃
        RefreshGuildMemberWeekActiveness( kfguild, kfguildmember );
        UpdateMemberActiveness( kfmsg.playerid(), kfmsg.guildid(), KFOperateEnum::Add, kfmsg.activeness() );
        UpdateGuildActiveness( kfmsg.guildid(), KFOperateEnum::Add, kfmsg.activeness() );

    }

    __KF_TIMER_FUNCTION__( KFGuildShardModule::OnTimerRefreshGuild )
    {
        auto* kfguild = _kf_component->FirstEntity();
        while ( kfguild != nullptr )
        {
            auto guildid = kfguild->GetKeyID();
            RefreshGuildApplicantlist( guildid );
            RefreshGuildWeekActiveness( kfguild );
            kfguild = _kf_component->NextEntity();
        }

    }
    void KFGuildShardModule::LoadGuildData()
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;

        auto kfglobal = KFGlobal::Instance();
        // 获取当前shard所存储的guildid
        auto queryguildlist = redisdriver->QueryList( "smembers {}:{}", __KF_STRING__( guildidset ), kfglobal->_app_id._union._id );
        if ( !queryguildlist->IsOk() || queryguildlist->_value.empty() )
        {
            return;
        }
        std::list<std::string> strguildids;
        for ( auto& strguildid : queryguildlist->_value )
        {
            strguildids.push_back( strguildid );
        }

        // 获取帮派信息
        auto cursor = _invalid_int;
        for ( auto& strguildid : strguildids )
        {
            if ( cursor % 10 == _invalid_int && _invalid_int != cursor )
            {
                auto result = redisdriver->PipelineListMap( );
                if ( !result->IsOk() || result->_value.empty() )
                {
                    __LOG_ERROR__( "load guild data failed!" );
                }
                LoadMultiGuildData( result->_value );
            }
            ++cursor;
            redisdriver->Append( "hgetall {}:{}", __KF_STRING__( guild ), strguildid );

        }
        if ( cursor > _invalid_int )
        {
            auto result = redisdriver->PipelineListMap( );
            if ( !result->IsOk() || result->_value.empty() )
            {
                // log error
            }
            LoadMultiGuildData( result->_value );
        }

    }

    void KFGuildShardModule::LoadMultiGuildData( std::list< MapString >& guildidlists )
    {
        for ( auto& iter : guildidlists )
        {
            LoadSingleGuildData( iter );
        }
    }

    void KFGuildShardModule::LoadSingleGuildData( MapString& guilds )
    {
        auto keyiter = guilds.find( __KF_STRING__( id ) );
        if ( keyiter == guilds.end() )
        {
            __LOG_ERROR__( "load single guild data failed! guildid lost" );
            return;
        }
        auto guildid = KFUtility::ToValue<uint64>( keyiter->second );
        auto kfguild = _kf_component->CreateEntity( guildid );
        auto kfobject = kfguild->GetData();

        // 这边加载帮派信息
        for ( auto& child : guilds )
        {
            kfobject->SetValue<std::string>( child.first, child.second );
        }

        // 这边加载帮派成员信息
        auto guildmemberlist = kfobject->GetValue<std::string>( __KF_STRING__( memberlist ) );
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ) );

        auto cursor = _invalid_int;
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        while ( !guildmemberlist.empty() )
        {
            if ( _invalid_int == cursor % 10 && _invalid_int != cursor )
            {
                auto result = redisdriver->PipelineListMap(  );
                if ( !result->IsOk() || result->_value.empty() )
                {
                    __LOG_ERROR__( "load single guild failed!" );
                    ++cursor;
                    continue;
                }
                LoadSingleGuildMember( kfobject, result->_value );

            }
            auto strmemberid = KFUtility::SplitString( guildmemberlist, DEFAULT_SPLIT_STRING );
            ++cursor;
            redisdriver->Append( "hgetall {}:{}", __KF_STRING__( guildmember ), strmemberid );

        }

        if ( cursor > _invalid_int )
        {
            auto result = redisdriver->PipelineListMap( );
            if ( !result->IsOk() || result->_value.empty() )
            {
                __LOG_ERROR__( "load single guild failed!" );
                return;
            }
            LoadSingleGuildMember( kfobject, result->_value );
        }
        kfguild->SetInited();

    }

    void KFGuildShardModule::LoadSingleGuildMember( KFData* kfdata, std::list< MapString >& memberlists )
    {
        for ( auto childmaps : memberlists )
        {
            auto kfmemberrecord = kfdata->FindData( __KF_STRING__( guildmember ) );
            auto kfmember = _kf_kernel->CreateObject( kfmemberrecord->GetDataSetting() );
            auto keyiter = childmaps.find( __KF_STRING__( id ) );
            if ( keyiter == childmaps.end() )
            {
                __LOG_ERROR__( "load single guild member failed! memberid lost" );
                return;
            }
            auto playerid = KFUtility::ToValue<uint64>( keyiter->second );
            kfmember->SetKeyID( playerid );
            for ( auto child : childmaps )
            {
                kfmember->SetValue< std::string >( child.first, child.second );
            }
            // 从数据库取basic 信息
            VectorString field;
            MapString values;
            auto kfbasics = kfmember->FindData( __KF_STRING__( basic ) );
            auto kfbasic = kfbasics->FirstData();
            while ( nullptr != kfbasic )
            {
                if ( kfbasic->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) )
                {
                    field.push_back( kfbasic->GetName() );
                }
                kfbasic = kfbasics->NextData();
            }

            if ( GetPlayBasic( playerid, values, field ) )
            {
                for ( auto& iter : values )
                {
                    auto kfdata = kfbasics->FindData( iter.first );
                    if ( kfdata != nullptr )
                    {
                        kfdata->SetValue<std::string>( iter.second );
                    }
                }
            }

            kfmemberrecord->AddData( playerid, kfmember );

        }
    }

    bool KFGuildShardModule::IsRepeateName( const std::string& name )
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "hexists {} {}",
                                                __KF_STRING__( guildnameidhash ), name );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "query guild name repeat failed! name[{}]", name );
            return true;
        }

        return result->_value != _invalid_int;
    }

    bool KFGuildShardModule::IsGuildMember( uint32 playerid )
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "exists {}:{}",
                                                __KF_STRING__( guildmember ), playerid );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "judge player[{}] in guild failed!", playerid );
        }

        return result->_value != _invalid_int;
    }

    bool KFGuildShardModule::IsInGuild( uint32 playerid, uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto kfmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        return kfmember != nullptr;

    }

    bool KFGuildShardModule::IsValidGuildId( uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        return true;
    }

    bool KFGuildShardModule::IsMaster( uint32 playerid, uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto masterid = kfobject->GetValue<uint32>( __KF_STRING__( masterid ) );
        return masterid == playerid;
    }

    bool KFGuildShardModule::IsViceMaster( uint32 playerid, uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        if ( nullptr == kfguildmember )
        {
            return false;
        }
        auto title = kfguildmember->GetValue<uint32>( __KF_STRING__( title ) );
        return title == KFGuildShardEnum::GuildViceMaster;
    }

    bool KFGuildShardModule::IsSameGuild( uint32 playerid, uint32 desplayerid, uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        auto deskfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), desplayerid );
        return ( nullptr != kfguildmember && nullptr != deskfguildmember );

    }

    bool KFGuildShardModule::IsInApplicanlist( uint64 guildid, uint32 playerid )
    {
        KFMsg::PBApplicationlists applylist;
        GetGuildApplicantlist( guildid, applylist );
        // applylist.ParseFromString( strapplicantlist );
        for ( auto i = 0; i < applylist.applylists_size(); ++i )
        {
            auto applydata = applylist.applylists( i );
            // 申请列表中已经有该玩家了
            if ( applydata.playerid() == playerid )
            {
                auto applytime = applydata.time();
                //存在申请列表中,但是不超过一天
                if ( !KFDate::CheckPassTime( applytime, KFTimeEnum::OneDaySecond ) )
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool KFGuildShardModule::JoinApplicanlist( uint64 guildid, uint32 playerid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }

        auto kfobject = kfguild->GetData();
        if ( nullptr == kfobject )
        {
            return false;
        }

        auto guildlevel = kfobject->GetValue<uint32>( __KF_STRING__( level ) );
        auto kfguildsetting = _kf_guild_shard_config->FindGuildSetting( guildlevel );
        if ( nullptr == kfguildsetting )
        {
            __LOG_ERROR__( "player[{}] join applicanlist failed! guild[{}] guild level[{}] setting:null", playerid, guildid, guildlevel );
            return false;
        }

        KFMsg::PBApplicationlists applylist;
        GetGuildApplicantlist( guildid, applylist );
        if ( kfguildsetting->_max_applylist <= static_cast<uint32>( applylist.applylists_size() ) )
        {
            return false;
        }

        auto pbapplylist =  applylist.add_applylists();
        pbapplylist->set_playerid( playerid );
        auto kfgobal = KFGlobal::Instance();
        pbapplylist->set_time( kfgobal->_real_time );
        auto basic = pbapplylist->mutable_basic();
        // 添加玩家基本信息
        MapString values;
        VectorString field;
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );

        auto kfguildmember = _kf_kernel->CreateObject( kfguildmembers->GetDataSetting() );
        if ( nullptr == kfguildmembers )
        {
            return false;
        }

        auto kfbasics = kfguildmember->FindData( __KF_STRING__( basic ) );
        if ( nullptr == kfbasics )
        {
            return false;
        }
        auto kfbasic = kfbasics->FirstData();
        while ( nullptr != kfbasic )
        {
            if ( kfbasic->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) )
            {
                field.push_back( kfbasic->GetName() );
            }
            kfbasic = kfbasics->NextData();
        }

        if ( GetPlayBasic( playerid, values, field ) )
        {
            for ( auto& iter : values )
            {
                auto pbstring = basic->add_pbstring();
                pbstring->set_name( iter.first );
                pbstring->set_value( iter.second );
            }
        }
        else
        {
            __LOG_ERROR__( "player[{}] join applicanlist failed! guild[{}]  basic is empty ", playerid, guildid );
            return false;
        }
        _kf_kernel->ReleaseObject( kfguildmember );

        auto newapplicantlist = KFProto::Serialize( &applylist, KFCompressEnum::Compress );
        if ( !newapplicantlist.empty() )
        {
            kfguild->UpdateData( __KF_STRING__( applicantlist ), newapplicantlist );
        }

        return true;
    }

    bool KFGuildShardModule::ClearApplicanlist( uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }
        kfguild->UpdateData( __KF_STRING__( applicantlist ), _invalid_str );
        return true;
    }


    void KFGuildShardModule::RefreshGuildApplicantlist( uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return;
        }
        KFMsg::PBApplicationlists applylist;
        GetGuildApplicantlist( guildid, applylist );
        KFMsg::PBApplicationlists newapplylist;
        auto ischange = false;
        for ( auto i = 0; i < applylist.applylists_size(); ++i )
        {
            auto applydata = applylist.applylists( i );
            auto playerid = applydata.playerid();
            if ( _invalid_int == playerid )
            {
                continue;
            }
            // TODO:玩家加入其他帮派的话，暂时清除申请列表
            if ( IsGuildMember( playerid ) )
            {
                continue;
            }

            auto applytime = applydata.time();
            //存在申请列表中,但是不超过一天
            if ( KFDate::CheckPassTime( applytime, KFTimeEnum::OneDaySecond ) )
            {
                ischange = true;
                continue;
            }
            auto pbapplylist = newapplylist.add_applylists();
            pbapplylist->CopyFrom( applydata );
        }
        auto newapplicantlist = KFProto::Serialize( &newapplylist, KFCompressEnum::Compress );
        if ( ischange )
        {
            kfguild->UpdateData( __KF_STRING__( applicantlist ), newapplicantlist );
        }
    }

    void KFGuildShardModule::RefreshGuildWeekActiveness( KFEntity* const kfguild )
    {
        if ( nullptr == kfguild )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            RefreshGuildMemberWeekActiveness( kfguild, kfguildmember );
            kfguildmember = kfguildmembers->NextData();
        }
    }

    void KFGuildShardModule::RefreshGuildMemberWeekActiveness( KFEntity* const kfguild, KFData* kfguildmember )
    {
        if ( nullptr == kfguildmember )
        {
            return;
        }
        auto lasttime = kfguildmember->GetValue<uint64>( __KF_STRING__( time ) );
        if ( !KFDate::CheckTime( 3, 0, 0, lasttime, KFGlobal::Instance()->_real_time ) )
        {
            return;
        }

        kfguild->UpdateData( kfguildmember, __KF_STRING__( time ), KFOperateEnum::Set, KFGlobal::Instance()->_real_time );
        kfguild->UpdateData( kfguildmember, __KF_STRING__( weekactiveness ), KFOperateEnum::Set, _invalid_int );
    }

    bool KFGuildShardModule::RemoveApplicanlist( uint64 guildid, uint32 playerid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }
        KFMsg::PBApplicationlists applylist;
        GetGuildApplicantlist( guildid, applylist );
        KFMsg::PBApplicationlists newapplylist;

        auto result = false;
        for ( auto i = 0; i < applylist.applylists_size(); ++i )
        {
            auto applydata = applylist.applylists( i );
            // 申请列表中已经有该玩家
            if ( applydata.playerid() == playerid )
            {
                auto applytime = applydata.time();
                //存在申请列表中,但是不超过一天
                if ( !KFDate::CheckPassTime( applytime, KFTimeEnum::OneDaySecond ) )
                {
                    result = true;
                }
                continue;
            }
            auto pbapplylist = newapplylist.add_applylists();
            pbapplylist->CopyFrom( applydata );
        }

        auto newapplicantlist = KFProto::Serialize( &newapplylist, KFCompressEnum::Compress );
        if ( !newapplicantlist.empty() )
        {
            kfguild->UpdateData( __KF_STRING__( applicantlist ), newapplicantlist );
        }

        return result;
    }


    uint32 KFGuildShardModule::JoinGuild( uint64 guildid, uint32 playerid, uint32 title )
    {
        if ( IsGuildMember( guildid ) )
        {
            // return KFMsg::Playerisguildmember;
        }
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        auto kfobject = kfguild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );

        auto level = kfobject->GetValue<uint32>( __KF_STRING__( level ) );
        auto kfguildsetting = _kf_guild_shard_config->FindGuildSetting( level );
        if ( nullptr == kfguildsetting )
        {
            __LOG_ERROR__( "player[{}] join guild[{}] failed! guild level[{}] setting:null", playerid, guildid, level );
            //return KFMsg::SysError;
        }

        if ( kfguildmembers->Size() >= kfguildsetting->_max_member )
        {
            //return KFMsg::GuildMemberlistTooLong;
        }
        auto kfguildmember = _kf_kernel->CreateObject( kfguildmembers->GetDataSetting() );

        auto kfglobal = KFGlobal::Instance();
        kfguildmember->SetKeyID( playerid );
        kfguildmember->SetValue<uint64>( __KF_STRING__( jointime ), kfglobal->_real_time );
        kfguildmember->SetValue<uint64>( __KF_STRING__( totalactiveness ), _invalid_int );
        kfguildmember->SetValue<uint32>( __KF_STRING__( title ), title );
        kfguildmember->SetValue<uint64>( __KF_STRING__( guildid ), guildid );

        auto kfbasics = kfguildmember->FindData( __KF_STRING__( basic ) );
        kfbasics->SetKeyID( playerid );
        MapString values;
        VectorString field;
        auto kfbasic = kfbasics->FirstData();
        while ( nullptr != kfbasic )
        {
            if ( kfbasic->HaveFlagMask( KFDataDefine::Mask_Guild_Data ) )
            {
                field.push_back( kfbasic->GetName() );
            }
            kfbasic = kfbasics->NextData();
        }
        auto serverid = _invalid_int;
        if ( GetPlayBasic( playerid, values, field ) )
        {
            for ( auto& iter : values )
            {
                if ( iter.first == __KF_STRING__( serverid ) )
                {
                    serverid = KFUtility::ToValue<uint32>( iter.second );
                }

                kfbasics->SetValue<std::string>( iter.first, iter.second );
            }
        }
        else
        {
            __LOG_ERROR__( "player[{}] join guild[{}] failed basic is empty ", playerid, guildid );
            // return KFMsg::SysError;
        }

        kfguild->AddData( kfguildmembers, kfguildmember );
        auto strmemberlist = kfobject->GetValue<std::string>( __KF_STRING__( memberlist ) );
        auto strplayerid = KFUtility::ToString( playerid );
        strmemberlist += ( strplayerid + DEFAULT_SPLIT_STRING );
        kfguild->UpdateData( __KF_STRING__( memberlist ), strmemberlist );

        auto ok = SendPlayerGuildChangeToClient( KFId( _invalid_int, serverid ), playerid, guildid, KFMsg::JoinGuild );
        if ( !ok )
        {
            SetPlayerGuildid( playerid, guildid );
        }


        return KFMsg::Ok;
    }

    bool KFGuildShardModule::GetPlayBasic( uint32 playerid, MapString& values, VectorString& field )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto querydata = publicredisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), playerid );
        if ( querydata->IsOk() )
        {
            if ( field.empty() )
            {
                values.swap( querydata->_value );
            }
            else
            {
                for ( auto iter : field )
                {
                    auto value = querydata->_value.find( iter );
                    if ( querydata->_value.end() != value )
                    {
                        values[value->first] = value->second;
                    }
                }
            }

        }
        return querydata->IsOk();
    }

    void KFGuildShardModule::MoveApplicantToGuild( uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return;
        }

        KFMsg::PBApplicationlists applylist;
        GetGuildApplicantlist( guildid, applylist );

        KFMsg::PBApplicationlists newapplylist;
        for ( auto i = 0; i < applylist.applylists_size(); ++i )
        {
            auto applydata = applylist.applylists( i );
            auto playerid = applydata.playerid();
            if ( _invalid_int == playerid )
            {
                continue;
            }

            auto applytime = applydata.time();
            //存在申请列表中,但是不超过一天
            if ( KFDate::CheckPassTime( applytime, KFTimeEnum::OneDaySecond ) )
            {
                continue;
            }

            if ( KFMsg::Ok == JoinGuild( guildid, playerid ) )
            {
                continue;
            }

            auto pbapplylist = newapplylist.add_applylists();
            pbapplylist->CopyFrom( applydata );
        }

        auto newapplicantlist = KFProto::Serialize( &newapplylist, KFCompressEnum::Compress );
        kfguild->UpdateData( __KF_STRING__( applicantlist ), newapplicantlist );

    }

    void  KFGuildShardModule::RemovePlayerGuildId( uint32 playerid )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto result = publicredisdriver->Execute( "hdel {}:{} {}",
                      __KF_STRING__( public ), playerid, __KF_STRING__( guildid ) );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "remove player[{}] guildid failed!", playerid );
        }
    }

    void  KFGuildShardModule::SetPlayerGuildid( uint32 playerid, uint64 guildid )
    {
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto result = publicredisdriver->Execute( "hset {}:{} {} {}",
                      __KF_STRING__( public ), playerid, __KF_STRING__( guildid ), guildid );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "set player[{}] guild id[{}] failed!", playerid, guildid );
        }
    }

    bool  KFGuildShardModule::QueryGuildListReq( uint32 cursor, KFMsg::MsgQueryGuildListAck* pbguilddatas )
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "zcard {}", __KF_STRING__( guildrank ) );
        auto pagecount =  _kf_guild_shard_config->GetGuildPage();
        if ( !result->IsOk() || result->_value < cursor ||  _invalid_int > pagecount )
        {
            return false;
        }

        auto kfresult = redisdriver->QueryList( "zrevrange {} {} {}", __KF_STRING__( guildrank ), cursor, cursor + pagecount );
        if ( !kfresult->IsOk() )
        {
            return false;
        }

        for ( auto& iter : kfresult->_value )
        {
            MapString guilddatas;
            if ( !QuerySingleGuildData( KFUtility::ToValue<uint64>( iter ), guilddatas ) )
            {
                continue;
            }
            SerialGuildData( guilddatas, pbguilddatas->add_guilddatas() );
        }
        return true;

    }

    bool KFGuildShardModule::QuerySingleGuildData( uint64 guildid, MapString& guilddatas )
    {
        guilddatas.clear();
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( guild ), guildid );
        if ( result->IsOk() && !result->_value.empty() )
        {
            guilddatas.swap( result->_value );
            return true;
        }
        return false;

    }

    uint32 KFGuildShardModule::GetTitle( uint64 playerid, uint64 guildid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return _invalid_int;
        }
        auto kfobject = kfguild->GetData();
        auto kfmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        return kfmember->GetValue<uint32>( __KF_STRING__( title ) );
    }

    bool KFGuildShardModule::IsGreatTitle( uint32 playerid, uint32 toplayerid, uint64 guildid )
    {
        if ( !IsSameGuild( playerid, toplayerid, guildid ) )
        {
            return false;
        }
        auto title = GetTitle( playerid, guildid );
        auto totitle = GetTitle( toplayerid, guildid );
        return title < totitle;

    }

    bool KFGuildShardModule::RemoveGuildMember( uint64 guildid, uint32 playerid, bool bekick )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( kfguild == nullptr )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto strmemberlist = kfobject->GetValue<std::string>( __KF_STRING__( memberlist ) );
        auto delplayerid = KFUtility::ToString( playerid ) + DEFAULT_SPLIT_STRING;
        std::string newmemberlist = "";
        if ( !KFUtility::DelString( strmemberlist, delplayerid, newmemberlist ) )
        {
            __LOG_ERROR__( "remove guild[{}] member failed! memberlist[{}] delplayer[{}]",
                           guildid, strmemberlist, playerid );
            return false;
        }
        // 扣除掉该玩家的周活跃度
        DecGuildMemberWeekActiveness( kfguild, playerid );

        kfguild->UpdateData( __KF_STRING__( memberlist ), newmemberlist );
        kfguild->RemoveData( __KF_STRING__( guildmember ), playerid );

        return true;
    }

    void KFGuildShardModule::DecGuildMemberWeekActiveness( KFEntity* kfguild, uint32 playerid )
    {
        auto kfobject = kfguild->GetData();
        auto totalactiveness = kfobject->GetValue<uint64>( __KF_STRING__( totalactiveness ) );
        if ( totalactiveness <= _invalid_int )
        {
            return;
        }

        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        if ( nullptr == kfguildmember )
        {
            return;
        }

        RefreshGuildMemberWeekActiveness( kfguild, kfguildmember );
        auto weekactiveness = kfguildmember->GetValue<uint64>( __KF_STRING__( weekactiveness ) );
        if ( _invalid_int >= weekactiveness )
        {
            return;
        }
        int restactiveness = ( int )totalactiveness - weekactiveness;
        auto lastvalue = __MAX__( restactiveness, _invalid_int );
        kfguild->UpdateData( __KF_STRING__( totalactiveness ), KFOperateEnum::Set, lastvalue );
    }


    void KFGuildShardModule::SerialGuildData( MapString& guilddata, KFMsg::PBStrings* pbguildata )
    {
        VectorString fields;
        auto kfguildclass = _kf_kernel->FindClassSetting( __KF_STRING__( guild ) );
        // 过滤需要查看的字段
        for ( auto& iter : kfguildclass->_static_data._objects )
        {
            if ( iter.second->HaveFlagMask( KFDataDefine::Mask_Sync_View ) )
            {
                fields.push_back( iter.first );
            }
        }
        if ( fields.empty() )
        {
            return;
        }

        for ( auto& iter : fields )
        {
            auto iterfield = guilddata.find( iter );
            if ( iterfield != guilddata.end() )
            {
                auto pbguild = pbguildata->add_pbstring();
                pbguild->set_name( iterfield->first );
                pbguild->set_value( iterfield->second );
            }
        }
    }

    bool KFGuildShardModule::SendMessageToGuildMember( uint32 playerid, uint64 guild, uint32 msgid, ::google::protobuf::Message* message, VectorString& offlineids, bool containself )
    {
        offlineids.clear();
        auto kfguild = _kf_component->FindEntity( guild, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }

        auto kfobject = kfguild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            auto memberid = kfguildmember->GetKeyID();
            if ( _invalid_int == memberid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            if ( playerid == memberid && !containself )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            if ( _invalid_int == serverid )
            {
                offlineids.push_back( KFUtility::ToString( memberid ) );
                kfguildmember = kfguildmembers->NextData();
                continue;
            }

            _kf_cluster_shard->SendToPlayer( serverid, memberid, msgid, message );

            kfguildmember = kfguildmembers->NextData();
        }
        return true;
    }

    uint32 KFGuildShardModule::GetServerid( uint32 playerid )
    {
        if ( playerid == _invalid_int )
        {
            return _invalid_int;
        }
        auto publicredisdriver = __PUBLIC_REDIS_DRIVER__;
        auto result = publicredisdriver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( public ), playerid, __KF_STRING__( serverid ) );
        if ( result->IsOk() )
        {
            return result->_value;
        }
        return _invalid_int;

    }

    uint64 KFGuildShardModule::GetGuildIdByPlayerid( uint32 playerid )
    {
        if ( playerid == _invalid_int )
        {
            return _invalid_int;
        }
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( guildmember ), playerid, __KF_STRING__( guildid ) );
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "player[{}] get guildid failed!", playerid );
            return _invalid_int;
        }
        auto guildid = KFUtility::ToValue<uint64>( result->_value );

        return guildid;
    }

    bool KFGuildShardModule::UpdateMemberActiveness( uint32 playerid, uint64 guildid, uint32 operate, uint32 value )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }
        auto kfobject = kfguild->GetData();
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), playerid );
        if ( kfguildmember == nullptr )
        {
            __LOG_ERROR__( "update player[{}] activeness failed! guild[{}]", playerid, guildid );
            return false;
        }
        kfguild->UpdateData( __KF_STRING__( guildmember ), playerid, __KF_STRING__( weekactiveness ), operate, value );

        return kfguild->UpdateData( __KF_STRING__( guildmember ), playerid, __KF_STRING__( activeness ), operate, value );

    }

    bool KFGuildShardModule::UpdateGuildActiveness( uint64 guildid, uint32 operate, uint32 value )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }

        return kfguild->UpdateData( __KF_STRING__( totalactiveness ), operate, value );

    }
    template<typename... Args>
    void KFGuildShardModule::WriteGuildLog( uint64 guildid, uint32 maxlog, uint32 logtype, Args&& ... args )
    {
        auto fmt =  _kf_guild_shard_config->GetGuildLogFmt( logtype );
        static const std::string _time_fmt = "[{}]";
        auto newfmt = _time_fmt + fmt;
        auto content = __FORMAT__( newfmt, KFDate::GetTimeString(), std::forward<Args>( args )... );
        KFUtility::ReplaceString( content, " ", "/" );
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        redisdriver->Append( "lpush {}:{} {}", __KF_STRING__( guildlog ), guildid, content );
        redisdriver->Append( "ltrim {}:{} {} {}", __KF_STRING__( guildlog ), guildid, _invalid_int, maxlog - 1 );
        auto result = redisdriver->Pipeline();
        if ( !result->IsOk() )
        {
            __LOG_ERROR__( "write guild[{}] log failed! content[{}]", guildid, content );
        }

    }

    bool KFGuildShardModule::SendPlayerGuildChangeToClient( const KFId& kfid, uint32 playerid, uint64 guildid, uint32 code )
    {
        KFMsg::S2SPlayerGuildChangeReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        req.set_code( code );
        return _kf_cluster_shard->SendToClient( kfid, KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ, &req );
    }

    void KFGuildShardModule::GetGuildApplicantlist( uint64 guildid, KFMsg::PBApplicationlists& applylist )
    {
        applylist.Clear();
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return;
        }
        auto kfobject = kfguild->GetData();
        auto strapplicantlist = kfobject->GetValue<std::string>( __KF_STRING__( applicantlist ) );
        if ( !strapplicantlist.empty() )
        {
            KFProto::Parse( &applylist, strapplicantlist, KFCompressEnum::Compress );
        }
    }


    bool KFGuildShardModule::SendMessageToGuildMember( uint64 guild, std::set<uint32>& titles, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfguild = _kf_component->FindEntity( guild, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return false;
        }

        auto kfobject = kfguild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            auto memberid = kfguildmember->GetKeyID();
            if ( _invalid_int == memberid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            auto title = kfguildmember->GetValue<uint32>( __KF_STRING__( title ) );
            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            if ( titles.end() != titles.find( title ) && _invalid_int != serverid )
            {
                _kf_cluster_shard->SendToPlayer( serverid, memberid, msgid, message );
                kfguildmember = kfguildmembers->NextData();
            }

            kfguildmember = kfguildmembers->NextData();
        }
        return true;
    }

    uint64 KFGuildShardModule::CalcGuildRankDataScore( KFData* kfparent )
    {
#define __RANK_SCORE_MULTIPLE__ 100000

        uint64 rankscore = 0u;
        auto i = 2u;

        if ( _kf_guild_shard_config->_show_data.empty() )
        {
            return rankscore;
        }


        for ( auto& showdata : _kf_guild_shard_config->_show_data )
        {
            if ( i == 0 )
            {
                break;
            }

            auto kfdata = kfparent->FindData( showdata );
            if ( kfdata != nullptr )
            {
                auto multiplevalue = pow( __RANK_SCORE_MULTIPLE__, --i );
                rankscore += static_cast<uint64>( kfdata->GetValue< double >() * multiplevalue );
            }
        }

        return rankscore;
    }


    void KFGuildShardModule::SendUpdateDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _update_print = 0;
        if ( _update_print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif
        auto kfobject = guild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            auto memberid = kfguildmember->GetKeyID();
            if ( _invalid_int == memberid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            if ( _invalid_int == serverid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            //KFMsg::MsgSyncUpdateGuildData sync;
            //sync.set_playerid( memberid );
            //sync.mutable_pbdata()->CopyFrom( pbobect );
            //_kf_cluster_shard->SendToClient( serverid, KFMsg::MSG_SYNC_UPDATE_GUILD_DATA, &sync );
            kfguildmember = kfguildmembers->NextData();
        }

    }


    void KFGuildShardModule::SendAddDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif
        auto kfobject = guild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            auto memberid = kfguildmember->GetKeyID();
            if ( _invalid_int == memberid )
            {
                kfguildmember = kfguildmembers->NextData();
                // log error playerid is empty
                continue;
            }
            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            if ( _invalid_int == serverid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            //KFMsg::MsgSyncAddGuildData sync;
            //sync.set_playerid( memberid );
            //sync.mutable_pbdata()->CopyFrom( pbobect );
            ////_kf_cluster_shard->SendToPlayer( serverid, memberid, msgid, message );
            //_kf_cluster_shard->SendToClient( serverid, KFMsg::MSG_SYNC_ADD_GUILD_DATA, &sync );
            //kfguildmember = kfguildmembers->NextData();
        }
        return;

    }

    void KFGuildShardModule::SendRemoveDataToClient( KFEntity* guild, const KFMsg::PBObject& pbobect )
    {
#ifdef __KF_DEBUG__
        static uint32 _print = 0;
        if ( _print == 1 )
        {
            auto temp = pbobect.DebugString();
        }
#endif
        auto kfobject = guild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = kfguildmembers->FirstData();
        while ( kfguildmember != nullptr )
        {
            auto memberid = kfguildmember->GetKeyID();
            if ( _invalid_int == memberid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            auto serverid = kfguildmember->GetValue<uint32>( __KF_STRING__( basic ), __KF_STRING__( serverid ) );
            if ( _invalid_int == serverid )
            {
                kfguildmember = kfguildmembers->NextData();
                continue;
            }
            //KFMsg::MsgSyncRemoveGuildData sync;
            //sync.set_playerid( memberid );
            //sync.mutable_pbdata()->CopyFrom( pbobect );
            ////_kf_cluster_shard->SendToPlayer( serverid, memberid, msgid, message );
            //_kf_cluster_shard->SendToClient( serverid, KFMsg::MSG_SYNC_REMOVE_GUILD_DATA, &sync );
            //kfguildmember = kfguildmembers->NextData();
        }
        return;

    }
}