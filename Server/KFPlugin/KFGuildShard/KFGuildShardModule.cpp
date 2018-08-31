#include "KFGuildShardModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __USE_WORKER__
#define __GUILD_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( guild ) )
#define __PUBLIC_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( public ) )

#ifdef __USE_WORKER__
    #define __REGISTER_GUILD_MESSAGE__ __REGISTER_WORKER_MESSAGE__
    #define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_WORKER_MESSAGE__
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_worker->SendMessageToClient( kfguid, msgid, message )
#else
    #define __REGISTER_GUILD_MESSAGE__ __REGISTER_MESSAGE__
    #define __UNREGISTER_GUILD_MESSAGE__ __UNREGISTER_MESSAGE__
    #define __SEND_MESSAGE_TO_CLIENT__( msgid, message ) _kf_cluster_shard->SendMessageToClient( kfguid, msgid, message )
#endif

#define __REDIS_PIPE_LIMIT_BEGIN__  auto cursor = _invalid_int
#define __REDIS_PIPE_PROCESS__	if ( _invalid_int == cursor % 10 && _invalid_int != cursor )\
    {\
        redisdriver->Pipeline();\
    }\
    ++cursor

#define __REDIS_PIPE_LIMIT_END__   if ( _invalid_int != ( cursor - 1 ) % 10 )\
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

    }

    void KFGuildShardModule::BeforeRun()
    {
        _kf_component = _kf_kernel->FindComponent( __KF_STRING__( guild ) );
        LoadGuildData();
        __REGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_TO_SHARD_REQ, &KFGuildShardModule::HandleCreateGuildToShard );
        __REGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_REQ, &KFGuildShardModule::HandleInviteGuildReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_REQ, &KFGuildShardModule::HandleApplyGuildReq );


        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MEMBER_BASIC_DATA_REQ, &KFGuildShardModule::HandleUpdateMemberBasic );

        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFGuildShardModule::OnServerDiscoverClient );

        _kf_component->SetEntityDataMask( __DELETE_AND_REMOVE__ );
        _kf_component->RegisterEntityDeleteFunction( this, &KFGuildShardModule::DeleteGuild );
        _kf_component->RegisterRemoveDataModule( this, &KFGuildShardModule ::OnRemoveDataCallBack );
        _kf_component->RegisterAddDataModule( this, &KFGuildShardModule::OnAddDataCallBack );
        _kf_component->RegisterUpdateDataModule( this, &KFGuildShardModule::OnUpdateDataCallBack );
        _kf_component->RegisterUpdateStringModule( this, &KFGuildShardModule::OnUpdateStringCallBack );

    }

    void KFGuildShardModule::BeforeShut()
    {
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CREATE_GUILD_TO_SHARD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_INVITE_GUILD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_MEMBER_BASIC_DATA_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_APPLY_GUILD_REQ );

        _kf_component->UnRegisterEntityDeleteFunction();
        _kf_component->UnRegisterRemoveDataModule( this );
        _kf_component->UnRegisterAddDataModule( this );
        _kf_component->UnRegisterUpdateDataModule( this );
        _kf_component->UnRegisterUpdateStringModule( this );

    }

    void KFGuildShardModule::OnceRun()
    {

    }

    __KF_SERVER_DISCOVER_FUNCTION__( KFGuildShardModule::OnServerDiscoverClient )
    {
        if ( handlename == __KF_STRING__( guild ) && handletype == __KF_STRING__( proxy ) )
        {
            std::list< uint64 > guildlist;

            auto* kfguild = _kf_component->FirstEntity();
            while ( kfguild != nullptr )
            {
                guildlist.push_back( kfguild->GetKeyID() );
                kfguild = _kf_component->NextEntity();
            }
            if ( !guildlist.empty() )
            {
                _kf_cluster_shard->AddObjectToProxy( handleid, guildlist );
            }
            else
            {
                _kf_cluster_shard->AddObjectToProxy( KFGlobal::Instance()->_app_id );
            }

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
        redisdriver->Append( "srem {} {}", __KF_STRING__( guildidset ), guildid );

        redisdriver->Pipeline( );

        //  remove proxy handle

        // 这边处理帮派玩家列表 // 发送到client 通知在线玩家帮派解散，操作离线玩家public guild字段,是否添加邮件?
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );

        auto kfguildmember = kfguildmembers->FirstData();

        __REDIS_PIPE_LIMIT_BEGIN__;
        while ( kfguildmember != nullptr )
        {
            if ( _invalid_int == kfguildmember->GetKeyID() )
            {
                kfguildmember = kfguildmembers->NextData();
                // log error playerid is empty
                continue;
            }

            __REDIS_PIPE_PROCESS__;

            redisdriver->Append( "del {}:{}", __KF_STRING__( guildmember ), kfguildmember->GetKeyID() );

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
            auto result = redisdriver->Execute( __FUNC_LINE__, "del {}:{}", kfdata->GetName(), key );
            if ( !result->IsOk() )
            {
                //log
            }
        }

        // 需要更新的属性
        if ( kfparent->HaveFlagMask( KFDataDefine::Mask_Sync_Client ) )
        {

        }

    }

    __KF_UPDATE_DATA_FUNCTION__( KFGuildShardModule::OnUpdateDataCallBack )
    {
        if ( !player->IsInited() )
        {
            return;
        }
        if ( kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) &&
                kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            auto redisdriver = __GUILD_REDIS_DRIVER__;
            auto kfresult = redisdriver->Execute( __FUNC_LINE__, "hset {}:{} {} {}",
                                                  kfdata->GetParent()->GetName(), key, kfdata->GetName(), newvalue );

        }

    }

    __KF_ADD_DATA_FUNCTION__( KFGuildShardModule::OnAddDataCallBack )
    {
        //
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
                    // log
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

        if ( kfdata->HaveFlagMask( KFDataDefine::Mask_Save_Database ) &&
                kfdata->GetParent()->HaveFlagMask( KFDataDefine::Mask_Save_Database ) )
        {
            auto redisdriver = __GUILD_REDIS_DRIVER__;
            auto kfresult = redisdriver->Execute( "hset {}:{} {} {}",
                                                  kfdata->GetParent()->GetName(), kfdata->GetParent()->GetKeyID(), kfdata->GetName(), kfdata->GetValue<std::string>() );
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
        if ( kfmsg.has_manifesto() )
        {
            ack.set_manifesto( kfmsg.manifesto() );
        }

        // 已经拥有帮派
        if ( IsGuildMember( kfmsg.playerid() ) )
        {
            ack.set_code( KFMsg::GuildHadBuild );
            _kf_cluster_shard->SendMessageToClient( __KF_HEAD_ID__( kfguid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
            return;
        }

        if ( IsRepeateName( kfmsg.guildname() ) )
        {
            // 名字重复
            ack.set_code( KFMsg::GuildNameRepeat );
            _kf_cluster_shard->SendMessageToClient( __KF_HEAD_ID__( kfguid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
            return;
        }


        // 记录帮派名字和id对应的hash， field->guildname, value->guildid
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        redisdriver->Append( "hset {} {} {}", __KF_STRING__( guildnameidhash ), kfmsg.guildname(), kfmsg.guildid() );
        // shardid->guildid
        auto kfglobal = KFGlobal::Instance();
        redisdriver->Append( "sadd {}:{} {}", __KF_STRING__( guildidset ), kfglobal->_app_id, kfmsg.guildid() );

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
        //values[ __KF_STRING__( memberlist ) ] = strplayerid + DEFAULT_SPLIT_STRING;
        values[ __KF_STRING__( totalactiveness ) ] = KFUtility::ToString( _invalid_int );
        if ( kfmsg.has_manifesto() )
        {
            values[ __KF_STRING__( manifesto ) ] = kfmsg.manifesto();
        }
        auto strguildid = KFUtility::ToString( kfmsg.guildid() );
        values[ __KF_STRING__( id ) ] = strguildid;
        for ( auto iter : values )
        {
            kfobject->SetValue< std::string >( iter.first, iter.second );
        }
        redisdriver->Append( values, "hmset {}:{}", __KF_STRING__( guild ), kfmsg.guildid() );
        redisdriver->Pipeline();
        // 这边设置完初始化标志后 会根据updata kfdata 回调去hset对应的键值对
        kfguild->SetInited();

        if ( !JoinGuild( kfmsg.guildid(), kfmsg.playerid(), KFGuildShardEnum::GuildMaster ) )
        {
            //log
        }

        /*
        // 添加成员信息
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfguildmember = _kf_kernel->CreateObject( kfguildmembers->GetDataSetting() );
        // 设置帮派成员(帮主)详细信息
        //auto kfguildmember = kfguild->CreateData( __KF_STRING__( guildmember ), kfmsg.playerid() );

        //auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ), kfmsg.playerid() );
        kfguildmember->SetKeyID( kfmsg.playerid() );
        kfguildmember->SetValue<uint64>( __KF_STRING__( jointime ), kfglobal->_real_time );
        kfguildmember->SetValue<uint64>( __KF_STRING__( totalactiveness ), _invalid_int );
        kfguildmember->SetValue<uint32>( __KF_STRING__( title ), KFGuildShardEnum::GuildManager );

        auto kfbasic = kfguildmember->FindData( __KF_STRING__( basic ) );
        kfbasic->SetKeyID( kfmsg.playerid() );
        for ( int i = 0; i < kfmsg.pbdata_size(); ++i )
        {
            auto pbstring = kfmsg.pbdata( i );
            kfbasic->SetValue<std::string>( pbstring.name(), pbstring.value() );
        }

        kfguild->AddData( kfguildmembers, kfguildmember );*/


        // 添加到代理
        std::list< uint64 > objectlist;
        objectlist.push_back( kfmsg.guildid() );
        _kf_cluster_shard->AddObjectToProxy( __KF_HEAD_ID__( kfguid ), objectlist );

        // 通知client 创建军团成功
        ack.set_code( KFMsg::Success );
        _kf_cluster_shard->SendMessageToClient( __KF_HEAD_ID__( kfguid ), kfmsg.serverid(), KFMsg::S2S_CREATE_GUILD_ACK, &ack );
        //__SEND_MESSAGE_TO_CLIENT__( KFMsg::S2S_CREATE_GUILD_ACK, &ack );
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
            return;
        }

        if ( IsInApplicanlist( kfmsg.guildid(), kfmsg.invitedid() ) )
        {
            return _kf_display->SendToGame( kfmsg.serverid(), kfmsg.invitor(), KFMsg::PlayerINApplicanlist );
        }

        KFMsg::S2SInviteGuildAck ack;
        ack.set_guildid( kfmsg.guildid() );
        ack.set_playerid( kfmsg.invitor() );
        ack.set_invitedid( kfmsg.invitedid() );
        _kf_cluster_shard->SendMessageToClient( __KF_HEAD_ID__( kfguid ), kfmsg.serverid(), KFMsg::S2S_INVITE_GUILD_ACK, &ack );
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
            if ( JoinGuild( kfmsg.guildid(), kfmsg.playerid() ) )
            {
                SendPlayerGuildChangeToClient( kfguid, kfmsg.playerid(), kfmsg.guildid(), KFMsg::JoinGuild );
                return;
            }
            else
            {
                // basic empty or memberlist is full
            }

        }
        // 如果是帮主邀请的 但是帮派成员满了就先加到申请列表中
        // 加入申请列表
        if ( !JoinApplicanlist( kfmsg.guildid(), kfmsg.playerid() ) )
        {
            return;
        }


        KFMsg::S2SApplyGuildAck ack;
        ack.set_guildid( kfmsg.guildid() );
        ack.set_playerid( kfmsg.playerid() );
        _kf_cluster_shard->SendMessageToClient( kfguid, KFMsg::S2S_APPLY_GUILD_ACK, &ack );

    }

    void KFGuildShardModule::LoadGuildData()
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;

        auto kfglobal = KFGlobal::Instance();
        // 获取当前shard所存储的guildid
        auto queryguildlist = redisdriver->QueryList( "smembers {}:{}", __KF_STRING__( guildidset ), kfglobal->_app_id );
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
                    // log error
                }
                LoadMultiGuildData( result->_value );
            }
            ++cursor;
            redisdriver->Append( "hgetall {}:{}", __KF_STRING__( guild ), strguildid );

        }
        if ( _invalid_int != ( cursor - 1 ) % 10 )
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
        for ( auto iter : guildidlists )
        {
            LoadSingleGuildData( iter );
        }
    }

    void KFGuildShardModule::LoadSingleGuildData( MapString& guilds )
    {
        auto keyiter = guilds.find( __KF_STRING__( id ) );
        if ( keyiter == guilds.end() )
        {
            // log error data
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
                    // log error
                    ++cursor;
                    continue;
                }
                LoadSingleGuildMember( kfobject, result->_value );

            }
            auto strmemberid = KFUtility::SplitString( guildmemberlist, DEFAULT_SPLIT_STRING );
            ++cursor;
            redisdriver->Append( "hgetall {}:{}", __KF_STRING__( guildmember ), strmemberid );

        }

        if ( _invalid_int != ( cursor - 1 ) % 10 )
        {
            auto result = redisdriver->PipelineListMap( );
            if ( !result->IsOk() || result->_value.empty() )
            {
                // log error
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
                // log error data
                return;
            }
            auto guildid = KFUtility::ToValue<uint64>( keyiter->second );
            kfmember->SetKeyID( guildid );
            for ( auto child : childmaps )
            {
                kfmember->SetValue< std::string >( child.first, child.second );
            }
            kfmemberrecord->AddData( guildid, kfmember );

        }
    }

    bool KFGuildShardModule::IsRepeateName( const std::string& name )
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "hexists {} {}",
                                                __KF_STRING__( guildnameidhash ), name );

        return result->_value != _invalid_int;
    }

    bool KFGuildShardModule::IsGuildMember( uint32 playerid )
    {
        auto redisdriver = __GUILD_REDIS_DRIVER__;
        auto result = redisdriver->QueryUInt64( "exists {}:{}",
                                                __KF_STRING__( guildmember ), playerid );

        return result->_value != _invalid_int;
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

    bool KFGuildShardModule::IsInApplicanlist( uint64 guildid, uint32 playerid )
    {
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        if ( nullptr == kfguild )
        {
            return true;
        }

        auto kfobject = kfguild->GetData();
        auto strapplicantlist = kfobject->GetValue<std::string>( __KF_STRING__( applicantlist ) );
        // 玩家申请列表 std::string转换成pb数据格式
        KFMsg::PBApplicationlists applylist;
        KFProto::Parse( &applylist, strapplicantlist, KFCompressEnum::Compress );
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
        auto strapplicantlist = kfobject->GetValue<std::string>( __KF_STRING__( applicantlist ) );
        // 玩家申请列表 std::string转换成pb数据格式
        KFMsg::PBApplicationlists applylist;
        if ( !strapplicantlist.empty() )
        {
            KFProto::Parse( &applylist, strapplicantlist, KFCompressEnum::Compress );
        }

        auto pbapplylist =  applylist.add_applylists();
        pbapplylist->set_playerid( playerid );
        auto kfgobal = KFGlobal::Instance();
        pbapplylist->set_time( kfgobal->_real_time );
        auto basic = pbapplylist->mutable_basic();
        // 添加玩家基本信息
        MapString values;
        VectorString field;
        /*field.push_back( __KF_STRING__( name ) );
        field.push_back( __KF_STRING__( sex ) );
        field.push_back( __KF_STRING__( icon ) );*/
        auto kfguildmember = kfobject->FindData( __KF_STRING__( guildmember ) );
        auto kfbasics = kfguildmember->FindData( __KF_STRING__( basic ) );
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
            // TODO:: log
            return false;
        }

        auto newapplicantlist = KFProto::Serialize( &applylist, KFCompressEnum::Compress );
        kfguild->UpdateData( __KF_STRING__( applicantlist ), newapplicantlist );
        return true;
    }

    bool KFGuildShardModule::JoinGuild( uint64 guildid, uint32 playerid, uint32 title )
    {
        if ( IsGuildMember( guildid ) )
        {
            return false;
        }
        auto kfguild = _kf_component->FindEntity( guildid, __FUNC_LINE__ );
        auto kfobject = kfguild->GetData();
        auto kfguildmembers = kfobject->FindData( __KF_STRING__( guildmember ) );

        // TODO 读取配置
        if ( kfguildmembers->Size() >= 30 )
        {
            return false;
        }
        auto kfguildmember = _kf_kernel->CreateObject( kfguildmembers->GetDataSetting() );

        auto kfglobal = KFGlobal::Instance();
        kfguildmember->SetKeyID( playerid );
        kfguildmember->SetValue<uint64>( __KF_STRING__( jointime ), kfglobal->_real_time );
        kfguildmember->SetValue<uint64>( __KF_STRING__( totalactiveness ), _invalid_int );
        kfguildmember->SetValue<uint32>( __KF_STRING__( title ), title );

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

        if ( GetPlayBasic( playerid, values, field ) )
        {
            for ( auto& iter : values )
            {
                kfbasics->SetValue<std::string>( iter.first, iter.second );
            }
        }
        else
        {
            // todo log
            return false;
        }

        kfguild->AddData( kfguildmembers, kfguildmember );
        auto strmemberlist = kfobject->GetValue<std::string>( __KF_STRING__( memberlist ) );
        auto strplayerid = KFUtility::ToString( playerid );
        strmemberlist += ( strplayerid + DEFAULT_SPLIT_STRING );
        kfguild->UpdateData( __KF_STRING__( memberlist ), strmemberlist );
        return true;
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

    bool KFGuildShardModule::SendGuildMemberToClient( uint32 playerid, uint64 guild, uint32 msgid, ::google::protobuf::Message* message, VectorString& offlineids, bool containself )
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
                // log error playerid is empty
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

            _kf_cluster_shard->SendMessageToPlayer( serverid, playerid, msgid, message );

            kfguildmember = kfguildmembers->NextData();
        }
        return true;
    }


    bool KFGuildShardModule::SendPlayerGuildChangeToClient( const KFGuid& kfguid, uint32 playerid, uint64 guildid, uint32 code )
    {
        KFMsg::S2SPlayerGuildChangeReq req;
        req.set_guildid( guildid );
        req.set_playerid( playerid );
        req.set_code( code );
        return _kf_cluster_shard->SendMessageToClient( kfguid, KFMsg::S2S_PLAYER_GUILD_CHANGE_REQ, &req );
    }
}