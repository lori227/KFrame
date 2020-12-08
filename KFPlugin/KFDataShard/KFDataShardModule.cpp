#include "KFDataShardModule.hpp"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
    void KFDataShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFDataShardModule, KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, KFMsg::S2SLoadPlayerToDataReq, HandleLoadPlayerToDataReq );
        __REGISTER_MESSAGE__( KFDataShardModule, KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, KFMsg::S2SSavePlayerToDataReq, HandleSavePlayerToDataReq );
        __REGISTER_MESSAGE__( KFDataShardModule, KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, KFMsg::S2SQueryPlayerToDataReq, HandleQueryPlayerToDataReq );

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
        for ( auto& iter : KFDataSaveConfig::Instance()->_settings._objects )
        {
            auto kfsetting = iter.second;
            if ( !kfsetting->_is_open )
            {
                continue;
            }

            // 创建数据执行器
            auto sort = kfsetting->_sort;
            auto kfexecute = _data_execute.Find( sort );
            if ( kfexecute != nullptr )
            {
                sort = _data_execute._objects.rbegin()->first + 1;
                __LOG_WARN__( "dataexecute=[{}] sort=[{}] already exist", kfsetting->_id, kfsetting->_sort );
            }

            switch ( kfsetting->_id )
            {
            case KFMsg::Redis:
                kfexecute = __KF_NEW__( KFRedisDataExecute );
                break;
            case KFMsg::Mongo:
                kfexecute = __KF_NEW__( KFMongoDataExecute );
                break;
            case KFMsg::MySQL:
                kfexecute = __KF_NEW__( KFMySQLDataExecute );
                break;
            default:
                __LOG_ERROR__( "dataexecute=[{}] not support", kfsetting->_id );
                break;
            }

            if ( kfexecute != nullptr )
            {
                kfexecute->InitExecute( kfsetting );
                _data_execute.Insert( sort, kfexecute );
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
        auto pblogin = &kfmsg->pblogin();

        KFMsg::S2SLoadPlayerToGameAck ack;
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        bool ok = LoadPlayerData( pblogin->zoneid(), pblogin->playerid(), ack.mutable_playerdata() );
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
    bool KFDataShardModule::LoadPlayerData( uint32 zoneid, uint64 playerid, KFMsg::PBObject* pbobject )
    {
        auto loadok = false;
        for ( auto& iter : _data_execute._objects )
        {
            loadok = false;
            auto dataexecute = iter.second;

            // 加载数据
            auto kfresult = dataexecute->LoadPlayerData( zoneid, playerid );
            if ( kfresult == nullptr )
            {
                continue;
            }

            loadok = kfresult->IsOk();
            if ( !kfresult->IsOk() || kfresult->_value.empty() )
            {
                continue;
            }

            // 反序列化数据
            auto ok = KFProto::Parse( pbobject, kfresult->_value, dataexecute->_kf_setting->_compress_type, true );
            if ( !ok )
            {
                __LOG_ERROR__( "database=[{}] player[{}:{}] parse failed", dataexecute->_kf_setting->_id, zoneid, playerid );
                continue;
            }
            else
            {
                __LOG_INFO__( "database=[{}] player[{}:{}] load ok", dataexecute->_kf_setting->_id, zoneid, playerid );
                return true;
            }
        }

        return loadok;
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject, uint32 saveflag )
    {
        // 保存数据
        for ( auto& iter : _data_execute._objects )
        {
            auto dataexecute = iter.second;
            dataexecute->SavePlayerData( zoneid, playerid, pbobject, saveflag );
        }

        return true;
    }
}