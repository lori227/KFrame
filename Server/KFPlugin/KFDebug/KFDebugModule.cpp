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

        __LOG_DEBUG__( KFLogEnum::Logic, "player[{}] debug command[{}]!", playerid, kfmsg.command() );

        // 调用注册的函数
        auto kffunction = _kf_debug_function.Find( kfmsg.command() );
        if ( kffunction != nullptr )
        {
            VectorString params;
            for ( auto i = 0; i < kfmsg.params_size(); ++i )
            {
                params.push_back( kfmsg.params( i ) );
            }

            return kffunction->_function( player, params );
        }

        static auto debugluafile = _kf_option->GetValue< std::string >( "debugluafile" );

        // 没有注册函数, 调用脚本
        auto paramsize = kfmsg.params_size();
        switch ( paramsize )
        {
        case 0:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid );
            break;
        case 1:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str() );
            break;
        case 2:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str(), kfmsg.params( 1 ).c_str() );
            break;
        case 3:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str(), kfmsg.params( 1 ).c_str(), kfmsg.params( 2 ).c_str() );
            break;
        case 4:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str(), kfmsg.params( 1 ).c_str(), kfmsg.params( 2 ).c_str(), kfmsg.params( 3 ).c_str() );
            break;
        case 5:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str(), kfmsg.params( 1 ).c_str(), kfmsg.params( 2 ).c_str(), kfmsg.params( 3 ).c_str(), kfmsg.params( 4 ).c_str() );
            break;
        case 6:
            _kf_lua->CallFunction( debugluafile, kfmsg.command(), playerid,
                                   kfmsg.params( 0 ).c_str(), kfmsg.params( 1 ).c_str(), kfmsg.params( 2 ).c_str(), kfmsg.params( 3 ).c_str(), kfmsg.params( 4 ).c_str(), kfmsg.params( 5 ).c_str() );
            break;
        default:
            break;
        }

    }


}