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

	bool KFLuaScript::LoadScript( const std::string& file )
	{
		if ( file.empty() )
		{
			return false;
		}

		ReleaseScript();

		auto result = _lua_state->LoadFile( file.c_str() );
		if ( result != 0 )
		{
			KFLogger::LogScript( KFLogger::Error, "Load [%s] Failed!", file.c_str() );
			return false;
		}

		_lua_file = file;
		_lua_state->Call( 0, 0 );
		return true;
	}
}