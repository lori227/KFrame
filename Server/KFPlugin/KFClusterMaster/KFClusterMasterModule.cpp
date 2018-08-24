#include "KFClusterMasterModule.h"
#include "KFClusterMasterManage.h"
#include "KFClusterMasterConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFClusterMasterModule::KFClusterMasterModule()
    {
        _cluster_serial = 0;
    }

    KFClusterMasterModule::~KFClusterMasterModule()
    {
    }

    void KFClusterMasterModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_cluster_config, false );
        ///////////////////////////////////////////////////////////////////////////////
    }

    void KFClusterMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterMasterModule::OnServerLostClusterProxy );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_REQ, &KFClusterMasterModule::HandleClusterRegisterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_REQ, &KFClusterMasterModule::HandleClusterUpdateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_REQ, &KFClusterMasterModule::HandleClusterAuthReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ALLOC_SHARD_REQ, &KFClusterMasterModule::HandleAllocShardReq );
    }

    void KFClusterMasterModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ALLOC_SHARD_REQ );

    }

    void KFClusterMasterModule::OnceRun()
    {
        auto kfsetting = _kf_cluster_config->FindClusterSetting( KFGlobal::Instance()->_app_name );
        if ( kfsetting == nullptr )
        {
            return;
        }

        _cluster_key = kfsetting->_key;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_LOST_FUNCTION__( KFClusterMasterModule::OnServerLostClusterProxy )
    {
        KFProxyData* kfgate = _kf_proxy_manage->FindProxyServer( handleid );
        if ( kfgate == nullptr )
        {
            return;
        }

        __LOG_DEBUG__( KFLogEnum::Logic, "[{}:{}:{}|{}:{}] Lost!",
                       kfgate->_name, kfgate->_type, kfgate->_id, kfgate->_ip, kfgate->_port );

        _kf_proxy_manage->RemoveProxyServer( handleid );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterRegisterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterRegisterReq );

        _kf_proxy_manage->AddProxyServer( kfmsg.type(), kfmsg.id(), kfmsg.name(), kfmsg.ip(), kfmsg.port() );

        SendAllocShardToProxy( kfmsg.id() );
        __LOG_DEBUG__( KFLogEnum::Logic, "[{}:{}:{}|{}:{}] discovered!",
                       kfmsg.name(), kfmsg.type(), kfmsg.id(), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterUpdateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterUpdateReq );

        _kf_proxy_manage->UpdateProxyServer( kfmsg.gateid(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterAuthReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthReq );
        uint32 handleid = __KF_HEAD_ID__( kfguid );

        __LOG_DEBUG__( KFLogEnum::Logic, "[{}] cluster[{}] key req!", kfmsg.clusterkey(), handleid );

        if ( kfmsg.clusterkey() != _cluster_key )
        {
            return __LOG_ERROR__( KFLogEnum::System, "[{}!={}] cluster[{}] key error!",
                                  kfmsg.clusterkey(), _cluster_key, handleid );
        }

        KFMsg::S2SClusterAuthAck ack;
        ack.set_clustertype( kfmsg.clustertype() );
        auto kfproxy = _kf_proxy_manage->HashProxyServer( handleid );
        if ( kfproxy == nullptr )
        {
            ack.set_token( "" );
            ack.set_ip( "" );
            ack.set_port( 0 );
            ack.set_name( "" );
            ack.set_type( "" );
            ack.set_id( 0 );

            __LOG_ERROR__( KFLogEnum::System, "cluster[{}] can't find proxy, handleid[{}]!",
                           kfmsg.clusterkey(), KFAppID::ToString( handleid ) );
        }
        else
        {
            // 创建token
            std::string token = MakeAuthToken( kfguid );
            ack.set_token( token );
            ack.set_ip( kfproxy->_ip );
            ack.set_port( kfproxy->_port );
            ack.set_name( kfproxy->_name );
            ack.set_type( kfproxy->_type );
            ack.set_id( kfproxy->_id );

            // Token发送给GateServer
            KFMsg::S2SClusterTokenReq tokenreq;
            tokenreq.set_token( token );
            tokenreq.set_gateid( handleid );
            _kf_tcp_server->SendNetMessage( kfproxy->_id, KFMsg::S2S_CLUSTER_TOKEN_REQ, &tokenreq );

            __LOG_DEBUG__( KFLogEnum::Logic, "[{}] cluster[{}] key ok!",
                           kfmsg.clusterkey(), handleid );
        }

        _kf_tcp_server->SendNetMessage( handleid, KFMsg::S2S_CLUSTER_AUTH_ACK, &ack );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    std::string KFClusterMasterModule::MakeAuthToken( const KFGuid& guid )
    {
        std::string headid = KFUtility::ToString( guid._head_id );
        std::string dataid = KFUtility::ToString( guid._data_id );
        std::string date = KFUtility::ToString( KFDate::GetTimeEx() );

        std::string temp = __FORMAT__( "{}:{}-{}:{}", headid, dataid, date, ++_cluster_serial );
        return KFCrypto::Md5Encode( temp );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleAllocShardReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAllocShardReq );

        auto shardid = __KF_HEAD_ID__( kfguid );

        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            auto objectid = kfmsg.objectid( i );

            _total_objects.insert( objectid );
            _shard_objects[ shardid ].insert( objectid );
        }

        BalanceAllocShard( shardid );

        // 发送给所有proxy
        SendAllocShardToProxy( _invalid_int );
    }

    std::set< uint64 > KFClusterMasterModule::GetShardObject( uint32 shardid )
    {
        std::set< uint64 > outvalue;
        for ( auto& iter : _object_to_shard )
        {
            if ( iter.second == shardid )
            {
                outvalue.insert( iter.first );
            }
        }

        return outvalue;
    }

    uint32 KFClusterMasterModule::GetMaxObjectShard()
    {
        auto maxcount = _invalid_int;
        auto shardid = _invalid_int;

        for ( auto& iter : _shard_objects )
        {
            auto objectlist = GetShardObject( iter.first );
            if ( objectlist.size() >= maxcount )
            {
                maxcount = objectlist.size();
                shardid = iter.first;
            }
        }

        return shardid;
    }

    void KFClusterMasterModule::BalanceAllocShard( uint32 shardid )
    {
        // 已经分配
        {
            auto objectlist = GetShardObject( shardid );
            if ( objectlist.size() >= 1 )
            {
                return;
            }
        }

        // 先分配
        for ( auto objectid : _total_objects )
        {
            auto allocshardid = _object_to_shard[ objectid ];
            if ( allocshardid != _invalid_int )
            {
                continue;
            }

            _object_to_shard[ objectid ] = shardid;
        }

        // 找到负载最大
        auto maxshardid = GetMaxObjectShard();
        auto objectlist = GetShardObject( maxshardid );
        if ( objectlist.size() <= 1 )
        {
            return;
        }

        auto iter = objectlist.begin();
        std::advance( iter, 1 );
        auto objectid = *iter;

        _object_to_shard[ objectid ] = shardid;
    }

    void KFClusterMasterModule::SendAllocShardToProxy( uint32 proxyid )
    {
        KFMsg::S2SAllocShardAck ack;

        for ( auto iter : _object_to_shard )
        {
            ack.add_objectid( iter.first );
            ack.add_shardid( iter.second );
        }

        if ( proxyid != _invalid_int )
        {
            _kf_tcp_server->SendNetMessage( proxyid, KFMsg::S2S_ALLOC_SHARD_ACK, &ack );
        }
        else
        {
            for ( auto iter : _kf_proxy_manage->_kf_proxy_list._objects )
            {
                _kf_tcp_server->SendNetMessage( iter.first, KFMsg::S2S_ALLOC_SHARD_ACK, &ack );
            }
        }
    }
}