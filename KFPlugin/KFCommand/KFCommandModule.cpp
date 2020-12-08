#include "KFCommandModule.hpp"

namespace KFrame
{
    void KFCommandModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFCommandModule, KFMsg::MSG_COMMAND_REQ, KFMsg::MsgCommandReq, HandleCommandReq );
    }

    void KFCommandModule::BeforeShut()
    {
        __UN_MESSAGE__( KFMsg::MSG_COMMAND_REQ );
    }

    /////////////////////////////////////////////////////////////////
    void KFCommandModule::BindFunction( const std::string& command, KFCommandFunction& function )
    {
        auto kffunction = _kf_command_function.Create( command );
        kffunction->_function = function;
    }

    void KFCommandModule::UnRegisterFunction( const std::string& command )
    {
        _kf_command_function.Remove( command );
    }

    __KF_MESSAGE_FUNCTION__( KFCommandModule::HandleCommandReq, KFMsg::MsgCommandReq )
    {
#ifndef __KF_DEBUG__
        return;
#endif // __KF_DEBUG__

        __ROUTE_FIND_PLAYER__;

        // 调用注册的函数
        std::string command = kfmsg->command();
        std::transform( command.begin(), command.end(), command.begin(), ::tolower );

        __LOG_DEBUG__( "player[{}] debug command[{}]", playerid, command );

        auto kffunction = _kf_command_function.Find( command );
        if ( kffunction != nullptr )
        {
            StringVector params;
            for ( auto i = 0; i < kfmsg->params_size(); ++i )
            {
                params.push_back( kfmsg->params( i ) );
            }

            return kffunction->_function( player, params );
        }
    }
}
