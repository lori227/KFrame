#include "KFAccountModule.hpp"

namespace KFrame
{
    void KFAccountModule::BeforeRun()
    {
        auto auth_database_type = KFGlobal::Instance()->GetUInt32( __STRING__( authdatabase ) );
        switch ( auth_database_type )
        {
        case KFDatabaseEnum::Mongo:
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

    void KFAccountModule::SaveChannelData( uint64 account_id, KFJson& channel_data )
    {
        _account_logic->SaveChannelData( account_id, channel_data );
    }

    StringMap KFAccountModule::QueryChannelData( uint64 account_id )
    {
        return _account_logic->QueryChannelData( account_id );
    }

    std::string KFAccountModule::MakeAccountToken( const std::string& account, uint32 channel, uint64 account_id, uint32 expire_time /* = 3600u */ )
    {
        return _account_logic->MakeAccountToken( account, channel, account_id, expire_time );
    }

    StringMap KFAccountModule::VerifyAccountToken( uint64 account_id, const std::string& token )
    {
        return _account_logic->VerifyAccountToken( account_id, token );
    }

    void KFAccountModule::SaveZoneToken( const std::string& account, uint32 channel, uint64 account_id, uint32 zone_id, const std::string& token, uint32 expire_time )
    {
        _account_logic->SaveZoneToken( account, channel, account_id, zone_id, token, expire_time );
    }

    StringMap KFAccountModule::VerifyZoneToken( uint64 account_id, uint32 zone_id, const std::string& token )
    {
        return _account_logic->VerifyZoneToken( account_id, zone_id, token );
    }

    void KFAccountModule::SaveLoginData( uint64 account_id, const std::string& ip, uint32 zone_id )
    {
        _account_logic->SaveLoginData( account_id, ip, zone_id );
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    std::tuple<uint64, bool> KFAccountModule::QueryCreatePlayer( uint64 account_id, uint32 zone_id )
    {
        return _account_logic->QueryCreatePlayer( account_id, zone_id );
    }

    // 更新玩家在线信息
    bool KFAccountModule::UpdateOnline( uint64 account_id, uint64 player_id, uint64 world_id, uint64 game_id )
    {
        return _account_logic->UpdateOnline( account_id, player_id, world_id, game_id );
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

    uint64 KFAccountModule::CheckAccountInBlackList( uint64 account_id )
    {
        return _account_logic->CheckAccountInBlackList( account_id );
    }

    bool KFAccountModule::AddAccountBlackList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        return _account_logic->AddAccountBlackList( account_id, account, channel, time, comment );
    }

    bool KFAccountModule::RemoveAccountBlackList( uint64 account_id )
    {
        return _account_logic->RemoveAccountBlackList( account_id );
    }

    StringMapList KFAccountModule::QueryAccountBlackList()
    {
        return _account_logic->QueryAccountBlackList();
    }

    bool KFAccountModule::CheckAccountInWhiteList( uint64 account_id )
    {
        return _account_logic->CheckAccountInWhiteList( account_id );
    }

    bool KFAccountModule::AddAccountWhiteList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        return _account_logic->AddAccountWhiteList( account_id, account, channel, time, comment );
    }

    bool KFAccountModule::RemoveAccountWhiteList( uint64 account_id )
    {
        return _account_logic->RemoveAccountWhiteList( account_id );
    }

    StringMapList KFAccountModule::QueryAccountWhiteList()
    {
        return _account_logic->QueryAccountWhiteList();
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFAccountModule::SaveWeiXinAccessToken( const std::string& machine_code, const std::string& openid,
            const std::string& scope, const std::string& access_token, uint32 expire_time )
    {
        return _account_logic->SaveWeiXinAccessToken( machine_code, openid, scope, access_token, expire_time );
    }

    StringMap KFAccountModule::QueryWeiXinAccessToken( const std::string& machine_code )
    {
        return _account_logic->QueryWeiXinAccessToken( machine_code );
    }

    void KFAccountModule::SaveWeiXinRefreshToken( const std::string& machine_code, const std::string& refresh_token )
    {
        return _account_logic->SaveWeiXinRefreshToken( machine_code, refresh_token );
    }

    std::string KFAccountModule::QueryWeiXinRefreshToken( const std::string& machine_code )
    {
        return _account_logic->QueryWeiXinRefreshToken( machine_code );
    }
}