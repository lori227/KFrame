#include "KFDebugModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    KFDebugModule::KFDebugModule()
    {
    }

    KFDebugModule::~KFDebugModule()
    {
    }

    void KFDebugModule::InitModule()
    {
    }

    void KFDebugModule::BeforeRun()
    {
        __REGISTER_MESSAGE__( KFMsg::MSG_DEBUG_COMMAND_REQ, &KFDebugModule::HandleDebugCommandReq );
    }

    void KFDebugModule::BeforeShut()
    {
        __UNREGISTER_MESSAGE__( KFMsg::MSG_DEBUG_COMMAND_REQ );
    }

    /////////////////////////////////////////////////////////////////
    void KFDebugModule::BindDebugFunction( const std::string& command, KFDebugFunction& function )
    {
        auto kffunction = _kf_debug_function.Create( command );
        kffunction->_function = function;
    }

    void KFDebugModule::UnRegisterDebugFunction( const std::string& command )
    {
        _kf_debug_function.Remove( command );
    }

    __KF_MESSAGE_FUNCTION__( KFDebugModule::HandleDebugCommandReq )
    {
        __CLIENT_PROTO_PARSE__( KFMsg::MsgDebugCommandReq );

#ifndef __KF_DEBUG__
        return;
#endif // __KF_DEBUG__

        // 调用注册的函数
        std::string command = kfmsg.command();
        std::transform( command.begin(), command.end(), command.begin(), ::tolower );

        __LOG_DEBUG__( "player[{}] debug command[{}]!", playerid, command );

        auto kffunction = _kf_debug_function.Find( command );
        if ( kffunction != nullptr )
        {
            VectorString params;
            for ( auto i = 0; i < kfmsg.params_size(); ++i )
            {
                params.push_back( kfmsg.params( i ) );
            }

            return kffunction->_function( player, params );
        }
    }
}
