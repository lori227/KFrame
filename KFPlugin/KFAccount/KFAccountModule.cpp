#include "KFAccountModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFAccountModule::BeforeRun()
    {
        auto authdatabasetype = KFGlobal::Instance()->GetUInt32( __STRING__( authdatabase ) );
        switch ( authdatabasetype )
        {
        case KFMsg::Mongo:
            _account_logic = __NEW_OBJECT__( KFAccountMongo );
            break;
        default:
            _account_logic = __NEW_OBJECT__( KFAccountRedis );
            break;
        }
    }

    void KFAccountModule::BeforeShut()
    {
        __DELETE_OBJECT__( _account_logic );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    StringMap KFAccountModule::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        return _account_logic->QueryCreateAccount( account, channel );
    }

    void KFAccountModule::SaveChannelData( uint64 accountid, KFJson& channeldata )
    {
        _account_logic->SaveChannelData( accountid, channeldata );
    }

    StringMap KFAccountModule::QueryChannelData( uint64 accountid )
    {
        return _account_logic->QueryChannelData( accountid );
    }

    std::string KFAccountModule::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime /* = 3600u */ )
    {
        return _account_logic->MakeAccountToken( account, channel, accountid, expiretime );
    }

    StringMap KFAccountModule::VerifyAccountToken( uint64 accountid, const std::string& token )
    {
        return _account_logic->VerifyAccountToken( accountid, token );
    }

    void KFAccountModule::SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zoneid, const std::string& token, uint32 expiretime )
    {
        _account_logic->SaveZoneToken( account, channel, accountid, zoneid, token, expiretime );
    }

    StringMap KFAccountModule::VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token )
    {
        return _account_logic->VerifyZoneToken( accountid, zoneid, token );
    }

    void KFAccountModule::SaveLoginData( uint64 accountid, const std::string& ip, uint32 zoneid )
    {
        _account_logic->SaveLoginData( accountid, ip, zoneid );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<uint64, bool> KFAccountModule::QueryCreatePlayer( uint64 accountid, uint32 zoneid )
    {
        return _account_logic->QueryCreatePlayer( accountid, zoneid );
    }

    // 更新玩家在线信息
    bool KFAccountModule::UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        return _account_logic->UpdateOnline( accountid, playerid, worldid, gameid );
    }
}