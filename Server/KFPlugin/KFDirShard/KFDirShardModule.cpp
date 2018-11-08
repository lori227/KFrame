#include "KFDirShardModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( dir ) )

    void KFDirShardModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 60000, &KFDirShardModule::OnTimerRegisterDirUrl );

        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ, &KFDirShardModule::HandleUpdateOnlineToDirReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_REMOVE_ONLINE_TO_DIR_REQ, &KFDirShardModule::HandleRemoveOnlineToDirReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( dirlist ), false, &KFDirShardModule::HandleQueryDirList );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDirShardModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ONLINE_TO_DIR_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_REMOVE_ONLINE_TO_DIR_REQ );
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( dirlist ) );
    }

    __KF_TIMER_FUNCTION__( KFDirShardModule::OnTimerRegisterDirUrl )
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

            __LOG_INFO__( "login gate [{}][{}:{}] register!", KFAppID::ToString( kfmsg.appid() ), kfmsg.ip(), kfmsg.port() );
        }

        kfdirdata->_zone_name = kfmsg.zonename();
        if ( kfmsg.onlinecount() < kfdirdata->_online_count )
        {
            kfdirdata->_online_count = kfmsg.onlinecount();
            kfdirdata->_app_id = kfmsg.appid();
            kfdirdata->_zone_ip = kfmsg.ip();
            kfdirdata->_zone_port = kfmsg.port();
        }

        // 保存到均衡表
        auto redisdriver = __DIR_REDIS_DRIVER__;
        redisdriver->Execute( "zincrby {} 0 {}", __KF_STRING__( zonebalance ), kfmsg.zoneid() );
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
                kfdirdata->_zone_ip = _invalid_str;
                kfdirdata->_zone_port = _invalid_int;
                kfdirdata->_online_count = std::numeric_limits<uint32>::max();
            }
        }
    }

    __KF_HTTP_FUNCTION__( KFDirShardModule::HandleQueryDirList )
    {
        static auto kfdiroption = _kf_option->FindOption( __KF_STRING__( dirtype ) );

        KFJson response;
        response.SetValue( __KF_STRING__( serverlisttype ), kfdiroption->_uint32_value );

        switch ( kfdiroption->_uint32_value )
        {
        case KFServerEnum::SelectServerData:
        {
            KFJson request( data );
            auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
            if ( zoneid == _invalid_int )
            {
                zoneid = BalanceAllocZoneId();
            }

            auto kfdirdata = _kf_dir_list.Find( zoneid );
            if ( kfdirdata == nullptr )
            {
                return _kf_http_server->SendResponseCode( KFMsg::CanNotFindLoginNode );
            }

            response.SetValue( __KF_STRING__( ip ), kfdirdata->_zone_ip );
            response.SetValue( __KF_STRING__( port ), kfdirdata->_zone_port );
        }
        break;
        default:
        {
            if ( _kf_dir_list._objects.empty() )
            {
                return _kf_http_server->SendResponseCode( KFMsg::CanNotFindLoginNode );
            }

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
        auto zonelist = redisdriver->QueryList( "zrange {} 0 -1", __KF_STRING__( zonebalance ) );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& strzoneid : zonelist->_value )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( strzoneid );
            auto kfdirdata = _kf_dir_list.Find( zoneid );
            if ( kfdirdata != nullptr )
            {
                redisdriver->Execute( "zincrby {} 1 {}", __KF_STRING__( zonebalance ), zoneid );
                return zoneid;
            }
        }

        return _invalid_int;
    }
}