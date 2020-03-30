#include "KFAccountLogic.hpp"

namespace KFrame
{
    std::string KFAccountLogic::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime )
    {
        auto md5temp = __FORMAT__( "{}:{}:{}", accountid, KFGlobal::Instance()->_game_time, ++_token_serial );
        return KFCrypto::Md5Encode( md5temp );
    }

}