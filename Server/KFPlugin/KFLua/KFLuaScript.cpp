#include "KFLuaScript.h"

namespace KFrame
{
    KFLuaScript::KFLuaScript() : _is_initialized( false )
    {

    }

    KFLuaScript::~KFLuaScript()
    {
        ReleaseScript();
    }

    void KFLuaScript::ReleaseScript()
    {
        if ( _lua_file.empty() )
        {
            return;
        }

        _lua_file.clear();
        _lua_state->PushNil();
    }

    void KFLuaScript::LoadScript( const std::string& file )
    {
        if ( file.empty() )
        {
            return;
        }

        ReleaseScript();

        auto result = _lua_state->LoadFile( file.c_str() );
        if ( result != 0 )
        {
            return KFLogger::LogScript( KFLogger::Error, "Load [%s] Failed!", file.c_str() );
        }

        _lua_file = file;
        _is_initialized = true;
        _lua_state->Call( 0, 0 );
    }
}