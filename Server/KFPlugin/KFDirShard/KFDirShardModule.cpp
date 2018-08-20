#include "KFDirShardModule.h"
#include "KFJson.h"
#include "KFDirConfig.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( dir ) )

    void KFDirShardModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_dir_config, true );
    }

    void KFDirShardModule::BeforeRun()
    {
        _dir_list_type = _kf_dir_config->GetDirListType( KFGlobal::Instance()->_app_channel );

        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, &KFDirShardModule::HandleUpdateOnlineToDirReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOVE_ONLINE_TO_DIR_REQ, &KFDirShardModule::HandleRemoveOnlineToDirReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( dirlist ), false, &KFDirShardModule::HandleQueryDirList );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirShardModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOVE_ONLINE_TO_DIR_REQ );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( dirlist ) );
    }

    void KFDirShardModule::OnceRun()
    {
        // 注册url
        auto dirlisturl = __FORMAT__( "{}{}", _kf_http_server->GetHttpUrl(), __KF_STRING__( dirlist ) );

        auto redisdriver = __DIR_REDIS_DRIVER__;
        redisdriver->Execute( "sadd {} {}", __KF_STRING__( dirurl ), dirlisturl );
    }

    __KF_MESSAGE_FUNCTION__( KFDirShardModule::HandleUpdateOnlineToDirReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateOnlineToDirReq );

        // 判断渠道, 渠道不同不能注册
        if ( KFGlobal::Instance()->_app_channel != kfmsg.zonechannel() )
        {
            return;
        }

        // 更新小区信息
        auto kfdirdata = _kf_dir_list.Find( kfmsg.zoneid() );
        if ( kfdirdata == nullptr )
        {
            kfdirdata = __KF_CREATE__( KFDirData );
            kfdirdata->_zone_id = kfmsg.zoneid();
            kfdirdata->_zone_channel = kfmsg.zonechannel();
            _kf_dir_list.Insert( kfmsg.zoneid(), kfdirdata );

            // 保存到均衡表
            auto redisdriver = __DIR_REDIS_DRIVER__;
            redisdriver->Execute( "zincrby {} 0 {}", __KF_STRING__( zonebalance ), kfmsg.zoneid() );
        }

        kfdirdata->_zone_name = kfmsg.zonename();
        if ( kfmsg.onlinecount() < kfdirdata->_online_count )
        {
            kfdirdata->_online_count = kfmsg.onlinecount();
            kfdirdata->_app_id = kfmsg.appid();
            kfdirdata->_zone_ip = kfmsg.ip();
            kfdirdata->_zone_port = kfmsg.port();
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDirShardModule::HandleRemoveOnlineToDirReq )
    {
        __PROTO_PARSE__( KFMsg::S2SRemoveOnlineToDirReq );

        for ( auto& iter : _kf_dir_list._objects )
        {
            auto kfdirdata = iter.second;
            if ( kfdirdata->_app_id == kfmsg.appid() )
            {
                kfdirdata->_app_id = _invalid_int;
                kfdirdata->_online_count = std::numeric_limits<uint32>::max();
                kfdirdata->_zone_ip = _invalid_str;
                kfdirdata->_zone_port = _invalid_int;
            }
        }
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryDirList )
    {
        KFJson response;
        response.SetValue( __KF_STRING__( serverlisttype ), _dir_list_type );

        switch ( _dir_list_type )
        {
        case __SELECT_SERVER_DATA__:
        {
            KFJson request( data );
            auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
            if ( zoneid == _invalid_int )
            {
                zoneid = BalanceAllocZoneId();
            }

            auto kfdirdata = _kf_dir_list.Find( zoneid );
            if ( kfdirdata != nullptr )
            {
                response.SetValue( __KF_STRING__( ip ), kfdirdata->_zone_ip );
                response.SetValue( __KF_STRING__( port ), kfdirdata->_zone_port );
            }
            else
            {
                response.SetValue( __KF_STRING__( ip ), _invalid_str );
                response.SetValue( __KF_STRING__( port ), _invalid_int );
            }
        }
        break;
        default:
        {
            // todo: 存在多线程问题, 需要优化
            KFJson kfdirlistjson;
            for ( auto& iter : _kf_dir_list._objects )
            {
                auto kfdirdata = iter.second;

                KFJson kfjson;
                kfjson.SetValue( __KF_STRING__( id ), kfdirdata->_zone_id );
                kfjson.SetValue( __KF_STRING__( type ), kfdirdata->_zone_channel );
                kfjson.SetValue( __KF_STRING__( name ), kfdirdata->_zone_name );
                kfjson.SetValue( __KF_STRING__( ip ), kfdirdata->_zone_ip );
                kfjson.SetValue( __KF_STRING__( port ), kfdirdata->_zone_port );

                kfdirlistjson.append( kfjson );
            }

            response.SetValue< Json::Value& >( __KF_STRING__( serverlist ), kfdirlistjson );
        }
        break;
        }

        return _kf_http_server->SendResponse( response );
    }

    uint32 KFDirShardModule::BalanceAllocZoneId()
    {
        // 选择一个最小人数的分区
        auto redisdriver = __DIR_REDIS_DRIVER__;
        auto zonelist = redisdriver->QueryList( "zrevrange {} 0 0", __KF_STRING__( zonebalance ) );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        auto zoneid = KFUtility::ToValue< uint32 >( zonelist->_value.front() );
        if ( zoneid == _invalid_int )
        {
            return _invalid_int;
        }

        redisdriver->Execute( "zincrby {} 1 {}", __KF_STRING__( zonebalance ), 1 );
        return zoneid;
    }
}