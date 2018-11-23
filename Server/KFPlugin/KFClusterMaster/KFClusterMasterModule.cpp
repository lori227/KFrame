#include "KFClusterMasterModule.h"
#include "KFClusterMasterManage.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFClusterMasterModule::BeforeRun()
    {
        __REGISTER_SERVER_LOST_FUNCTION__( &KFClusterMasterModule::OnServerLostClusterProxy );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_REQ, &KFClusterMasterModule::HandleClusterRegisterReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_REQ, &KFClusterMasterModule::HandleClusterUpdateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_REQ, &KFClusterMasterModule::HandleClusterAuthReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_ALLOC_OBJECT_TO_MASTER_REQ, &KFClusterMasterModule::HandleAllocObjectToMasterReq );
    }

    void KFClusterMasterModule::BeforeShut()
    {
        __UNREGISTER_SERVER_LOST_FUNCTION__();

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_REGISTER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_UPDATE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_CLUSTER_AUTH_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ALLOC_OBJECT_TO_MASTER_REQ );
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

        __LOG_DEBUG__( "[{}:{}:{}|{}:{}] lost!", kfgate->_name, kfgate->_type, KFAppID::ToString( kfgate->_id ), kfgate->_ip, kfgate->_port );

        _kf_proxy_manage->RemoveProxyServer( handleid );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterRegisterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterRegisterReq );

        SendAllocShardToProxy( kfmsg.id() );
        __LOG_DEBUG__( "[{}:{}:{}|{}:{}] discovered!", kfmsg.name(), kfmsg.type(), KFAppID::ToString( kfmsg.id() ), kfmsg.ip(), kfmsg.port() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterUpdateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterUpdateReq );

        _kf_proxy_manage->AddProxyServer( kfmsg.type(), kfmsg.id(), kfmsg.name(), kfmsg.ip(), kfmsg.port(), kfmsg.count() );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleClusterAuthReq )
    {
        __PROTO_PARSE__( KFMsg::S2SClusterAuthReq );
        uint32 handleid = __KF_HEAD_ID__( kfid );

        __LOG_DEBUG__( "[{}] cluster[{}] key req!", kfmsg.clusterkey(), KFAppID::ToString( handleid ) );

        static auto _cluster_key = _kf_option->GetString( __KF_STRING__( clusterkey ), KFGlobal::Instance()->_app_name );
        if ( kfmsg.clusterkey() != _cluster_key )
        {
            return __LOG_ERROR__( "[{}!={}] cluster[{}] key error!", kfmsg.clusterkey(), _cluster_key, KFAppID::ToString( handleid ) );
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

            __LOG_ERROR__( "cluster[{}] can't find proxy, handleid[{}]!", kfmsg.clusterkey(), KFAppID::ToString( handleid ) );
        }
        else
        {
            // 创建token
            std::string token = MakeAuthToken( kfid );
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

            __LOG_DEBUG__( "[{}] cluster[{}] key ok!", kfmsg.clusterkey(), KFAppID::ToString( handleid ) );
        }

        _kf_tcp_server->SendNetMessage( handleid, KFMsg::S2S_CLUSTER_AUTH_ACK, &ack );
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    std::string KFClusterMasterModule::MakeAuthToken( const KFId& kfid )
    {
        std::string headid = __TO_STRING__( kfid._head_id );
        std::string dataid = __TO_STRING__( kfid._data_id );
        std::string date = __TO_STRING__( KFDate::GetTimeEx() );

        std::string temp = __FORMAT__( "{}:{}-{}:{}", headid, dataid, date, ++_cluster_serial );
        return KFCrypto::Md5Encode( temp );
    }

    __KF_MESSAGE_FUNCTION__( KFClusterMasterModule::HandleAllocObjectToMasterReq )
    {
        __PROTO_PARSE__( KFMsg::S2SAllocObjectToMasterReq );

        auto shardid = __KF_HEAD_ID__( kfid );

        for ( auto i = 0; i < kfmsg.objectid_size(); ++i )
        {
            auto objectid = kfmsg.objectid( i );

            _total_objects.insert( objectid );
            _shard_objects[ shardid ].insert( objectid );
        }

        BalanceAllocShard( shardid );

        // 发送给shard
        SendAllocShardToShard();

        // 发送给所有proxy
        SendAllocShardToProxy( _invalid_int );
    }

    std::set< uint32 > KFClusterMasterModule::GetShardObject( uint32 shardid )
    {
        std::set< uint32 > outvalue;
        for ( auto& iter : _object_to_shard )
        {
            if ( iter.second == shardid )
            {
                outvalue.insert( iter.first );
            }
        }

        return outvalue;
    }

    void KFClusterMasterModule::BalanceAllocShard( uint32 shardid )
    {
        _object_to_shard.clear();

        std::vector< uint32 > shardlist;
        for ( auto iter : _shard_objects )
        {
            shardlist.push_back( iter.first );
        }

        auto allocindex = 0u;
        for ( auto objectid : _total_objects )
        {
            if ( allocindex >= shardlist.size() )
            {
                allocindex = 0u;
            }

            auto shardid = shardlist[ allocindex ];
            if ( !HaveObject( shardid, objectid ) )
            {
                shardid = FindShard( objectid );
            }
            else
            {
                allocindex++;
            }

            _object_to_shard[ objectid ] = shardid;
        }
    }

    bool KFClusterMasterModule::HaveObject( uint32 shardid, uint32 objectid )
    {
        return _shard_objects[ shardid ].count( objectid ) > 0;
    }

    uint32 KFClusterMasterModule::FindShard( uint32 objectid )
    {
        for ( auto iter : _shard_objects )
        {
            if ( iter.second.count( objectid ) > 0 )
            {
                return iter.first;
            }
        }

        return _invalid_int;
    }

    void KFClusterMasterModule::SendAllocShardToProxy( uint32 proxyid )
    {
        KFMsg::S2SAllocObjectToProxyAck ack;

        for ( auto iter : _object_to_shard )
        {
            ack.add_objectid( iter.first );
            ack.add_shardid( iter.second );
        }

        if ( proxyid != _invalid_int )
        {
            _kf_tcp_server->SendNetMessage( proxyid, KFMsg::S2S_ALLOC_OBJECT_TO_PROXY_ACK, &ack );
        }
        else
        {
            _kf_tcp_server->SendMessageToType( __KF_STRING__( proxy ), KFMsg::S2S_ALLOC_OBJECT_TO_PROXY_ACK, &ack );
        }
    }

    void KFClusterMasterModule::SendAllocShardToShard()
    {
        for ( auto& iter : _shard_objects )
        {
            auto shardid = iter.first;
            auto objectlist = GetShardObject( shardid );

            KFMsg::S2SAllocObjectToShardAck ack;
            for ( auto objectid : objectlist )
            {
                ack.add_objectid( objectid );
            }
            _kf_tcp_server->SendNetMessage( shardid, KFMsg::S2S_ALLOC_OBJECT_TO_SHARD_ACK, &ack );
        }
    }
}