#include "KFAccountMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    StringMap KFAccountMongo::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        StringMap accountdata;

        return accountdata;
    }

    void KFAccountMongo::SaveChannelData( uint64 accountid, KFJson& channeldata )
    {
        if ( !__JSON_HAS_MEMBER__( channeldata, __STRING__( extend ) ) )
        {
            return;
        }

        auto& kfextend = channeldata[ __STRING__( extend ) ];

        StringMap values;
        __JSON_TO_MAP__( kfextend, values );

    }

    StringMap KFAccountMongo::QueryChannelData( uint64 accountid )
    {
        return StringMap();
    }

    std::string KFAccountMongo::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime )
    {
        auto token = KFAccountLogic::MakeAccountToken( account, channel, accountid, expiretime );
        return token;
    }

    StringMap KFAccountMongo::VerifyAccountToken( uint64 accountid, const std::string& token )
    {

        static StringMap _result;
        return _result;
    }

    void KFAccountMongo::SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zoneid, const std::string& token, uint32 expiretime )
    {

    }

    StringMap KFAccountMongo::VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token )
    {

        static StringMap _result;
        return _result;
    }

    void KFAccountMongo::SaveLoginData( uint64 accountid, const std::string& ip, uint32 zoneid )
    {

    }

    std::tuple<uint64, bool> KFAccountMongo::QueryCreatePlayer( uint64 accountid, uint32 zoneid )
    {

        return std::make_tuple( 0, false );
    }

    // 更新玩家在线信息
    bool KFAccountMongo::UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        return true;
    }
}