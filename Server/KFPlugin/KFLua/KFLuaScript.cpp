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

        try
        {
            _lua_file.clear();
            _lua_state->PushNil();
        }
        catch ( ... )
        {
            __LOG_ERROR__( "release [{}] failed unknown!", _lua_file );
        }
    }

    void KFLuaScript::LoadScript( const std::string& file )
    {
        if ( file.empty() )
        {
            return;
        }

        ReleaseScript();

        try
        {
            auto result = _lua_state->LoadFile( file.c_str() );
            if ( result != 0 )
            {
                return __LOG_ERROR__( "Load [{}] Failed!", file );
            }

            _lua_state->Call( 0, 0 );

            _lua_file = file;
            _is_initialized = true;
        }
        catch ( LuaPlus::LuaException& e )
        {
            __LOG_ERROR__( "load [{}] failed=[{}]", file, e.GetErrorMessage() );
        }
        catch ( ... )
        {
            __LOG_ERROR__( "load [{}] failed unknown!", file  );
        }
    }
}