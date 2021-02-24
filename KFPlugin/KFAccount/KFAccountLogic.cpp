#include "KFAccountLogic.hpp"

namespace KFrame
{
    std::string KFAccountLogic::MakeAccountToken( const std::string& account, uint32 channel, uint64 account_id, uint32 expire_time )
    {
        auto source = __FORMAT__( "{}:{}:{}", account_id, KFGlobal::Instance()->_game_time, ++_token_serial );
        return KFCrypto::Md5Encode( source );
    }

}