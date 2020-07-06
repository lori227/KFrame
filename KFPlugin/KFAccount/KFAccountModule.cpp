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

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFAccountModule::CheckIpInBlackList( const std::string& ip )
    {
        return _account_logic->CheckIpInBlackList( ip );
    }

    bool KFAccountModule::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        return _account_logic->AddIpBlackList( ip, time, comment );
    }

    bool KFAccountModule::RemoveIpBlackList( const std::string& ip )
    {
        return _account_logic->RemoveIpBlackList( ip );
    }

    StringMapList KFAccountModule::QueryIpBlackList()
    {
        return _account_logic->QueryIpBlackList();
    }

    bool KFAccountModule::CheckIpInWhiteList( const std::string& ip )
    {
        return _account_logic->CheckIpInWhiteList( ip );
    }

    bool KFAccountModule::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        return _account_logic->AddIpWhiteList( ip, time, comment );
    }

    bool KFAccountModule::RemoveIpWhiteList( const std::string& ip )
    {
        return _account_logic->RemoveIpWhiteList( ip );
    }

    StringMapList KFAccountModule::QueryIpWhiteList()
    {
        return _account_logic->QueryIpWhiteList();
    }

    uint64 KFAccountModule::CheckAccountInBlackList( uint64 accountid )
    {
        return _account_logic->CheckAccountInBlackList( accountid );
    }

    bool KFAccountModule::AddAccountBlackList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        return _account_logic->AddAccountBlackList( accountid, account, channel, time, comment );
    }

    bool KFAccountModule::RemoveAccountBlackList( uint64 accountid )
    {
        return _account_logic->RemoveAccountBlackList( accountid );
    }

    StringMapList KFAccountModule::QueryAccountBlackList()
    {
        return _account_logic->QueryAccountBlackList();
    }

    bool KFAccountModule::CheckAccountInWhiteList( uint64 accountid )
    {
        return _account_logic->CheckAccountInWhiteList( accountid );
    }

    bool KFAccountModule::AddAccountWhiteList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        return _account_logic->AddAccountWhiteList( accountid, account, channel, time, comment );
    }

    bool KFAccountModule::RemoveAccountWhiteList( uint64 accountid )
    {
        return _account_logic->RemoveAccountWhiteList( accountid );
    }

    StringMapList KFAccountModule::QueryAccountWhiteList()
    {
        return _account_logic->QueryAccountWhiteList();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAccountModule::SaveWeiXinAccessToken( const std::string& machinecode, const std::string& openid,
            const std::string& scope, const std::string& accesstoken, uint32 expirestime )
    {
        return _account_logic->SaveWeiXinAccessToken( machinecode, openid, scope, accesstoken, expirestime );
    }

    StringMap KFAccountModule::QueryWeiXinAccessToken( const std::string& machinecode )
    {
        return _account_logic->QueryWeiXinAccessToken( machinecode );
    }

    void KFAccountModule::SaveWeiXinRefreshToken( const std::string& machinecode, const std::string& refreshtoken )
    {
        return _account_logic->SaveWeiXinRefreshToken( machinecode, refreshtoken );
    }

    std::string KFAccountModule::QueryWeiXinRefreshToken( const std::string& machinecode )
    {
        return _account_logic->QueryWeiXinRefreshToken( machinecode );
    }
}