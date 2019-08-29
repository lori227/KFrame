#include "KFDataShardModule.hpp"
#include "KFUtility/KFCompress.h"

namespace KFrame
{
    void KFDataShardModule::BeforeRun()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_LOAD_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleLoadPlayerToDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_SAVE_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleSavePlayerToDataReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_QUERY_PLAYER_TO_DATA_REQ, &KFDataShardModule::HandleQueryPlayerToDataReq );

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
        for ( auto& iter : KFDataShardConfig::Instance()->_settings._objects )
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
                __LOG_WARN__( "dataexecute=[{}] sort=[{}] already exist!", kfsetting->_id, kfsetting->_sort );
            }

            if ( kfsetting->_id == "redis" )
            {
                kfexecute = __KF_NEW__( KFRedisDataExecute );
            }
            else if ( kfsetting->_id == "mongo" )
            {
                kfexecute = __KF_NEW__( KFMongoDataExecute );
            }
            else if ( kfsetting->_id == "mysql" )
            {
                kfexecute = __KF_NEW__( KFMySQLDataExecute );
            }
            else
            {
                __LOG_ERROR__( "dataexecute=[{}] not support!" );
                continue;
            }

            kfexecute->InitExecute( kfsetting );
            _data_execute.Insert( sort, kfexecute );
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
    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleSavePlayerToDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SSavePlayerToDataReq );

        auto zoneid = KFGlobal::Instance()->UUIDZoneId( KFMsg::UUidPlayer, kfmsg.id() );
        auto ok = SavePlayerData( zoneid, kfmsg.id(), &kfmsg.data(), kfmsg.flag() );
        if ( ok )
        {
            KFMsg::S2SSavePlayerToGameAck ack;
            ack.set_id( kfmsg.id() );
            _kf_route->SendToRoute( route, KFMsg::S2S_SAVE_PLAYER_TO_GAME_ACK, &ack );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleLoadPlayerToDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SLoadPlayerToDataReq );

        auto pblogin = &kfmsg.pblogin();
        auto zoneid = KFGlobal::Instance()->UUIDZoneId( KFMsg::UUidPlayer, pblogin->playerid() );
        __LOG_INFO__( "player[{}:{}:{}:{}] load req!", pblogin->account(), pblogin->accountid(), pblogin->playerid(), zoneid );

        KFMsg::S2SLoadPlayerToGameAck ack;
        ack.mutable_pblogin()->CopyFrom( *pblogin );
        bool ok = LoadPlayerData( zoneid, pblogin->playerid(), ack.mutable_playerdata() );
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

    __KF_MESSAGE_FUNCTION__( KFDataShardModule::HandleQueryPlayerToDataReq )
    {
        __PROTO_PARSE__( KFMsg::S2SQueryPlayerToDataReq );

        auto zoneid = KFGlobal::Instance()->UUIDZoneId( KFMsg::UUidPlayer, kfmsg.playerid() );

        KFMsg::S2SQueryPlayerToGameAck ack;
        auto ok = LoadPlayerData( zoneid, kfmsg.playerid(), ack.mutable_playerdata() );
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
            auto ok = KFProto::Parse( pbobject, kfresult->_value, KFCompressEnum::Compress );
            if ( !ok )
            {
                __LOG_ERROR__( "database=[{}] player[{}:{}] parse failed!", dataexecute->GetName(), zoneid, playerid );
                continue;
            }
            else
            {
                __LOG_INFO__( "database=[{}] player[{}:{}] load ok!", dataexecute->GetName(), zoneid, playerid );
                return true;
            }
        }

        return loadok;
    }

    bool KFDataShardModule::SavePlayerData( uint32 zoneid, uint64 playerid, const KFMsg::PBObject* pbobject, uint32 saveflag )
    {
        // 序列化数据
        auto strdata = KFProto::Serialize( pbobject, KFCompressEnum::Compress );
        if ( strdata == _invalid_str )
        {
            __LOG_ERROR__( "player[{}:{}] serialize failed!", zoneid, playerid );
            return false;
        }

        // 保存数据
        for ( auto& iter : _data_execute._objects )
        {
            auto dataexecute = iter.second;
            dataexecute->SavePlayerData( zoneid, playerid, strdata, saveflag );
        }

        return true;
    }
}