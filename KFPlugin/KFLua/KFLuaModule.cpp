#include "KFLuaModule.hpp"

namespace KFrame
{
    void KFLuaModule::BeforeRun()
    {
        _player_component = _kf_kernel->FindComponent( __STRING__( player ) );
        __REGISTER_DEPLOY_FUNCTION__( __STRING__( loadscript ), &KFLuaModule::LoadScript );
    }

    void KFLuaModule::ShutDown()
    {
        __UN_DEPLOY_FUNCTION__( __STRING__( loadscript ) );
    }

    __KF_DEPLOY_FUNCTION__( KFLuaModule::LoadScript )
    {
        if ( param == _globbing_string )
        {
            _lua_script.Clear();
        }
        else
        {
            auto lua_script = _lua_script.Find( param );
            if ( lua_script != nullptr )
            {
                lua_script->LoadScript( param );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFLuaScript> KFLuaModule::FindLuaScript( const std::string& lua_file )
    {
        auto lua_script = _lua_script.Create( lua_file );
        if ( !lua_script->_is_initialized )
        {
            // 注册函数
            RegisterLuaFunction( lua_script );

            // 加载文件
            lua_script->LoadScript( lua_file );
        }

        return lua_script;
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1, uint64 param2 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1, param2 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1, uint64 param2, uint64 param3 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1, param2, param3 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1, uint64 param2, uint64 param3, uint64 param4 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1, param2, param3, param4 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1, param2, param3, param4, param5 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5, uint64 param6 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1, param2, param3, param4, param5, param6 );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str() );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1, const std::string& param2 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str(), param2.c_str() );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1, const std::string& param2, const std::string& param3 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str(), param2.c_str(), param3.c_str() );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1, const std::string& param2, const std::string& param3,
                              const std::string& param4 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str(), param2.c_str(), param3.c_str(),
                                          param4.c_str() );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1, const std::string& param2, const std::string& param3,
                              const std::string& param4, const std::string& param5 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str(), param2.c_str(), param3.c_str(),
                                          param4.c_str(), param5.c_str() );
    }

    uint64 KFLuaModule::Call( const std::string& lua_file, const std::string& function, uint64 object_id,
                              const std::string& param1, const std::string& param2, const std::string& param3,
                              const std::string& param4, const std::string& param5, const std::string& param6 )
    {
        auto lua_script = FindLuaScript( lua_file );
        return lua_script->CallFunction( function, object_id, param1.c_str(), param2.c_str(), param3.c_str(),
                                          param4.c_str(), param5.c_str(), param6.c_str() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __LUA_FUNCTION__( name, function ) \
    meta_table_object.RegisterObjectDirect( name, this, function );


    void KFLuaModule::RegisterLuaFunction( std::shared_ptr<KFLuaScript> lus_script )
    {
        auto meta_table_object = lus_script->_lua_state->GetGlobals().CreateTable( "MultiObjectMetaTable" );
        meta_table_object.SetObject( "__index", meta_table_object );

        __LUA_FUNCTION__( "LogDebug", &KFLuaModule::LuaLogDebug );
        __LUA_FUNCTION__( "LogInfo", &KFLuaModule::LuaLogInfo );
        __LUA_FUNCTION__( "LogWarn", &KFLuaModule::LuaLogWarn );
        __LUA_FUNCTION__( "LogError", &KFLuaModule::LuaLogError );
        __LUA_FUNCTION__( "Md5Encode", &KFLuaModule::LuaMd5Encode );
        __LUA_FUNCTION__( "GetValue", &KFLuaModule::LuaGetValue );
        __LUA_FUNCTION__( "OperateValue", &KFLuaModule::LuaOperateValue );
        __LUA_FUNCTION__( "GetObjectValue", &KFLuaModule::LuaGetObjectValue );
        __LUA_FUNCTION__( "OperateObjectValue", &KFLuaModule::LuaOperateObjectValue );
        __LUA_FUNCTION__( "GetRecordValue", &KFLuaModule::LuaGetRecordValue );
        __LUA_FUNCTION__( "OperateRecordValue", &KFLuaModule::LuaOperateRecordValue );
        __LUA_FUNCTION__( "GetVariable", &KFLuaModule::LuaGetVariable );
        __LUA_FUNCTION__( "OperateVariable", &KFLuaModule::LuaOperateVariable );
        __LUA_FUNCTION__( "GetGameTime", &KFLuaModule::LuaGetGameTime );
        __LUA_FUNCTION__( "GetRealTime", &KFLuaModule::LuaGetRealTime );
        __LUA_FUNCTION__( "AddData", &KFLuaModule::LuaAddData );
        __LUA_FUNCTION__( "SetData", &KFLuaModule::LuaSetData );
        __LUA_FUNCTION__( "DecData", &KFLuaModule::LuaDecData );
        __LUA_FUNCTION__( "STHttpGet", &KFLuaModule::LuaSTGet );
        __LUA_FUNCTION__( "STHttpPost", &KFLuaModule::LuaSTPost );
        __LUA_FUNCTION__( "MTHttpGet", &KFLuaModule::LuaMTGet );
        __LUA_FUNCTION__( "MTHttpPost", &KFLuaModule::LuaMTPost );
        __LUA_FUNCTION__( "GetConstantString", &KFLuaModule::LuaGetConstantString );
        __LUA_FUNCTION__( "GetConstantUInt32", &KFLuaModule::LuaGetConstantUInt32 );

        LuaPlus::LuaObject lua_object = lus_script->_lua_state->BoxPointer( this );
        lua_object.SetMetatable( meta_table_object );
        lus_script->_lua_state->GetGlobals().SetObject( "KFrame", lua_object );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLuaModule::LuaLogDebug( const char* data )
    {
        __LOG_DEBUG__( "{}", data );
    }

    void KFLuaModule::LuaLogInfo( const char* data )
    {
        __LOG_INFO__( "{}", data );
    }

    void KFLuaModule::LuaLogWarn( const char* data )
    {
        __LOG_WARN__( "{}", data );
    }

    void KFLuaModule::LuaLogError( const char* data )
    {
        __LOG_ERROR__( "{}", data );
    }

    const char* KFLuaModule::LuaMd5Encode( const char* data )
    {
        static std::string _result;
        _result = KFCrypto::Md5Encode( data );
        return _result.c_str();
    }

    uint64 KFLuaModule::LuaGetValue( uint64 object_id, const char* data_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->Get( data_name );
    }

    uint64 KFLuaModule::LuaOperateValue( uint64 object_id, const char* data_name, uint32 operate, uint64 value )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->UpdateData( data_name, operate, value );
    }

    uint64 KFLuaModule::LuaGetObjectValue( uint64 object_id, const char* parent_name, const char* data_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->Get( parent_name, data_name );
    }

    uint64 KFLuaModule::LuaOperateObjectValue( uint64 object_id, const char* parent_name, const char* data_name, uint32 operate, uint64 value )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->UpdateObject( parent_name, data_name, operate, value );
    }

    uint64 KFLuaModule::LuaGetRecordValue( uint64 object_id, const char* parent_name, uint64 key, const char* data_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->Get( parent_name, key, data_name );
    }

    uint64 KFLuaModule::LuaOperateRecordValue( uint64 object_id, const char* parent_name, uint64 key, const char* data_name, uint32 operate, uint64 value )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return _invalid_int;
        }

        return entity->UpdateRecord( parent_name, key, data_name, operate, value );
    }

    uint32 KFLuaModule::LuaGetVariable( uint32 variable_id )
    {
        return _run_variable[ variable_id ];
    }

    // 操作运行时变量
    uint32 KFLuaModule::LuaOperateVariable( uint32 variable_id, uint32 operate, uint32 value )
    {
        auto base = LuaGetVariable( variable_id );
        auto final = KFUtility::Operate( base, operate, value );
        _run_variable[ variable_id ] = final;

        return final;
    }

    uint64 KFLuaModule::LuaGetGameTime()
    {
        return KFGlobal::Instance()->_game_time;
    }

    uint64 KFLuaModule::LuaGetRealTime()
    {
        return KFGlobal::Instance()->_real_time;
    }

    void KFLuaModule::LuaAddData( uint64 object_id, const char* element, const char* module_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = element;
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            entity->AddElement( &elements, _default_multiple, module_name, 0u, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaSetData( uint64 object_id, const char* element, const char* module_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = element;
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            elements.SetOperate( KFEnum::Set );
            entity->AddElement( &elements, _default_multiple, module_name, 0u, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaDecData( uint64 object_id, const char* element, const char* module_name )
    {
        auto entity = _player_component->FindEntity( object_id, __FUNC_LINE__ );
        if ( entity == nullptr )
        {
            return;
        }

        KFElements elements;
        elements._str_parse = element;
        auto ok = KFGlobal::Instance()->ParseElement( elements, __FILE__, __LINE__ );
        if ( ok )
        {
            entity->RemoveElement( &elements, _default_multiple, module_name, 0u, __FUNC_LINE__ );
        }
    }

    const char* KFLuaModule::LuaSTGet( const char* url, const char* data )
    {
        static std::string _result = "";
        _result = _kf_http_client->STGet( url, data );
        return _result.c_str();
    }

    const char* KFLuaModule::LuaSTPost( const char* url, const char* data )
    {
        static std::string _result = "";
        _result = _kf_http_client->STPost( url, data );
        return _result.c_str();
    }

    void KFLuaModule::LuaMTGet( const char* url, const char* data, const char* args )
    {
        _kf_http_client->MTGet( url, data, this, &KFLuaModule::OnLuaHttpCallBack, args );
    }

    void KFLuaModule::LuaMTPost( const char* url, const char* data, const char* args )
    {
        _kf_http_client->MTPost( url, data, this, &KFLuaModule::OnLuaHttpCallBack, args );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLuaModule::OnLuaHttpCallBack )
    {
        if ( pass_back_data.empty() )
        {
            return;
        }

        __JSON_PARSE_STRING__( kfjson, pass_back_data );

        auto lua_file = __JSON_GET_STRING__( kfjson, __STRING__( luafile ) );
        auto lua_function = __JSON_GET_STRING__( kfjson, __STRING__( luafunction ) );
        if ( lua_file.empty() || lua_function.empty() )
        {
            return;
        }

        auto object_id = __JSON_GET_UINT64__( kfjson, __STRING__( playerid ) );
        Call( lua_file, lua_function, object_id, send_data, recv_data );
    }

    const char* KFLuaModule::LuaGetConstantString( const char* name, uint32 logic_id )
    {
        auto& value = KFGlobal::Instance()->GetString( name, logic_id );
        return value.c_str();
    }

    uint32 KFLuaModule::LuaGetConstantUInt32( const char* name, uint32 logic_id )
    {
        return KFGlobal::Instance()->GetUInt32( name, logic_id );
    }
}