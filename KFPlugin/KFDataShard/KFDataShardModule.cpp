#include "KFDataShardModule.hpp"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
    void KFDataShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleLoadPlayerToDataReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleSavePlayerToDataReq );
        __REGISTER_MESSAGE__( KFMessageEnum::Normal, KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleQueryPlayerToDataReq );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFDataShardModule::BeforeShut()
    {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UN_MESSAGE__( KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ );
        __UN_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ );
        __UN_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFDataShardModule::PrepareRun()
    {
        for ( auto& iter : KFDataSaveConfig::Instance()->_setting_list._objects )
        {
            auto setting = iter.second;
            if ( !setting->_is_open )
            {
                continue;
            }

            // 创建数据执行器
            auto sort = setting->_sort;
            auto execute_data = _data_execute.Find( sort );
            if ( execute_data != nullptr )
            {
                sort = _data_execute._objects.rbegin()->first + 1;
                __LOG_WARN__( "data execute=[{}] sort=[{}] already exist", setting->_id, setting->_sort );
            }

            switch ( setting->_id )
            {
            case KFDatabaseEnum::Redis:
                execute_data = __MAKE_SHARED__( KFRedisDataExecute );
                break;
            case KFDatabaseEnum::Mongo:
                execute_data = __MAKE_SHARED__( KFMongoDataExecute );
                break;
            case KFDatabaseEnum::MySQL:
                execute_data = __MAKE_SHARED__( KFMySQLDataExecute );
                break;
            default:
                __LOG_ERROR__( "data_execute=[{}] not support", setting->_id );
                break;
            }

            if ( execute_data != nullptr )
            {
                execute_data->InitExecute( setting );
                _data_execute.Insert( sort, execute_data );
            }
        }
    }

    void KFDataShardModule::Run()
    {
        for ( auto& iter : _data_execute._objects )
        {
            auto dataexecute = iter.second;
            dataexecute->RunDataKeeper();
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSavePlayerToDataReq, KFMsg::S2SSavePlayerToDataReq )
    {
        auto ok = SavePlayerData( kfmsg->zoneid(), kfmsg->id(), &kfmsg->data(), kfmsg->flag() );
        if ( ok )
        {
            KFMsg::S2SSavePlayerToGameAck ack;
            ack.set_id( kfmsg->id() );
            _kf_route->SendToRoute( route, KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoadPlayerToDataReq, KFMsg::S2SLoadPlayerToDataReq )
    {
        auto login_data = &kfmsg->pblogin();

        KFMsg::S2SLoadPlayerToGameAck ack;
        ack.mutable_pblogin()->CopyFrom( *login_data );
        bool ok = LoadPlayerData( login_data->zoneid(), login_data->playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Ok );
        }
        else
        {
            ack.set_result( KFMsg::LoginLoadDataFailed );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_LOAD_PLAYER_TO_GAME_ACK, &ack );
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleQueryPlayerToDataReq, KFMsg::S2SQueryPlayerToDataReq )
    {
        KFMsg::S2SQueryPlayerToGameAck ack;
        auto ok = LoadPlayerData( kfmsg->zoneid(), kfmsg->playerid(), ack.mutable_playerdata() );
        if ( ok )
        {
            ack.set_result( KFMsg::Ok );
        }
        else
        {
            ack.set_result( KFMsg::QueryPlayerFailed );
        }

        _kf_route->SendToRoute( route, KFMsg::S2S_QUERY_PLAYER_TO_GAME_ACK, &ack );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFDataShardModule::LoadPlayerData( uint32 zone_id, uint64 player_id, KFMsg::PBObject* proto_object )
    {
        auto load_ok = false;
        for ( auto& iter : _data_execute._objects )
        {
            load_ok = false;
            auto data_execute = iter.second;

            // 加载数据
            auto result = data_execute->LoadPlayerData( zone_id, player_id );
            if ( result == nullptr )
            {
                continue;
            }

            load_ok = result->IsOk();
            if ( !result->IsOk() || result->_value.empty() )
            {
                continue;
            }

            // 反序列化数据
            auto ok = KFProto::Parse( proto_object, result->_value, data_execute->_data_save_setting->_compress_type, true );
            if ( !ok )
            {
                __LOG_ERROR__( "database=[{}] player[{}:{}] parse failed", data_execute->_data_save_setting->_id, zone_id, player_id );
                continue;
            }
            else
            {
                __LOG_INFO__( "database=[{}] player[{}:{}] load ok", data_execute->_data_save_setting->_id, zone_id, player_id );
                return true;
            }
        }

        return load_ok;
    }

    bool KFDataShardModule::SavePlayerData( uint32 zone_id, uint64 player_id, const KFMsg::PBObject* player_object, uint32 save_flag )
    {
        // 保存数据
        for ( auto& iter : _data_execute._objects )
        {
            auto data_execute = iter.second;
            data_execute->SavePlayerData( zone_id, player_id, player_object, save_flag );
        }

        return true;
    }
}