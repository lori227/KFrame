#include "KFPublicShardModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFPublicShardModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_PUBLIC_REQ, &KFPublicShardModule::HandleUpdateDataToPublicReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_PUBLIC_REQ, &KFPublicShardModule::HandleQueryBasicToPublicReq );
    }

    void KFPublicShardModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_DATA_TO_PUBLIC_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_QUERY_BASIC_TO_PUBLIC_REQ );
    }

    void KFPublicShardModule::OnceRun()
    {
        // 初始化redis
        _name_redis_driver = _kf_redis->Create( __KF_STRING__( name ) );
        _public_redis_driver = _kf_redis->Create( __KF_STRING__( public ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleUpdateDataToPublicReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateDataToPublicReq );

        auto playerid = __ROUTE_SEND_ID__;
        auto pbdata = &kfmsg.pbdata();

        MapString values;
        values.insert( pbdata->begin(), pbdata->end() );
        _public_redis_driver->Update( values, "hmset {}:{}", __KF_STRING__( public ), playerid );
    }

    __KF_MESSAGE_FUNCTION__( KFPublicShardModule::HandleQueryBasicToPublicReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryBasicToPublicReq );

        KFMsg::S2SQueryBasicToGameAck ack;
        ack.set_name( kfmsg.name() );

        auto queryid = _name_redis_driver->QueryUInt64( "get {}:{}:{}", __KF_STRING__( player ), __KF_STRING__( name ), kfmsg.name() );
        if ( !queryid->IsOk() )
        {
            ack.set_result( KFMsg::NameDatabaseBusy );
        }
        else
        {
            if ( queryid->_value == _invalid_int )
            {
                ack.set_result( KFMsg::QueryBasicNotExist );
            }
            else
            {
                auto querydata = _public_redis_driver->QueryMap( "hgetall {}:{}", __KF_STRING__( public ), queryid->_value );
                if ( !querydata->IsOk() )
                {
                    ack.set_result( KFMsg::PublicDatabaseBusy );
                }
                else
                {
                    auto pbdata = ack.mutable_pbdata();
                    for ( auto& iter : querydata->_value )
                    {
                        ( *pbdata )[ iter.first ] = iter.second;
                    }
                }
            }
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_BASIC_TO_GAME_ACK, &ack );
    }
}