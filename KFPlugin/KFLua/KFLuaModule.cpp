#include "KFLuaModule.hpp"

namespace KFrame
{
    void KFLuaModule::BeforeRun()
    {
        _kf_player = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_DEPLOY_FUNCTION__( __KF_STRING__( loadscript ), &KFLuaModule::LoadScript );
    }

    void KFLuaModule::ShutDown()
    {
        __UNREGISTER_DEPLOY_FUNCTION__( __KF_STRING__( loadscript ) );
    }

    __KF_DEPLOY_FUNCTION__( KFLuaModule::LoadScript )
    {
        if ( param == _globbing_str )
        {
            _lua_script.Clear();
        }
        else
        {
            auto kfluascript = _lua_script.Find( param );
            if ( kfluascript != nullptr )
            {
                kfluascript->LoadScript( param );
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFLuaScript* KFLuaModule::FindLuaScript( const std::string& luafile )
    {
        auto kfluascript = _lua_script.Create( luafile );
        if ( !kfluascript->_is_initialized )
        {
            // 注册函数
            RegisterLuaFunction( kfluascript );

            // 加载文件
            kfluascript->LoadScript( luafile );
        }

        return kfluascript;
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1, uint64 param2 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1, param2 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1, uint64 param2, uint64 param3 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1, param2, param3 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1, uint64 param2, uint64 param3, uint64 param4 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1, param2, param3, param4 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1, param2, param3, param4, param5 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5, uint64 param6 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1, param2, param3, param4, param5, param6 );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str() );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1, const std::string& param2 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str(), param2.c_str() );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1, const std::string& param2, const std::string& param3 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str(), param2.c_str(), param3.c_str() );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1, const std::string& param2, const std::string& param3,
                            const std::string& param4 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str(), param2.c_str(), param3.c_str(),
                                   param4.c_str() );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1, const std::string& param2, const std::string& param3,
                            const std::string& param4, const std::string& param5 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str(), param2.c_str(), param3.c_str(),
                                   param4.c_str(), param5.c_str() );
    }

    void KFLuaModule::Call( const std::string& luafile, const std::string& function, uint64 objectid,
                            const std::string& param1, const std::string& param2, const std::string& param3,
                            const std::string& param4, const std::string& param5, const std::string& param6 )
    {
        auto kfluascript = FindLuaScript( luafile );
        kfluascript->CallFunction( function, objectid, param1.c_str(), param2.c_str(), param3.c_str(),
                                   param4.c_str(), param5.c_str(), param6.c_str() );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define __LUA_FUNCTION__( name, function ) \
    metatableobject.RegisterObjectDirect( name, this, function );


    void KFLuaModule::RegisterLuaFunction( KFLuaScript* kfscript )
    {
        auto metatableobject = kfscript->_lua_state->GetGlobals().CreateTable( "MultiObjectMetaTable" );
        metatableobject.SetObject( "__index", metatableobject );

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
        __LUA_FUNCTION__( "GetOptionString", &KFLuaModule::LuaGetOptionString );
        __LUA_FUNCTION__( "GetOptionUInt32", &KFLuaModule::LuaGetOptionUInt32 );

        LuaPlus::LuaObject kframeobject = kfscript->_lua_state->BoxPointer( this );
        kframeobject.SetMetatable( metatableobject );
        kfscript->_lua_state->GetGlobals().SetObject( "KFrame", kframeobject );
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

    uint64 KFLuaModule::LuaGetValue( uint64 objectid, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( dataname );
    }

    uint64 KFLuaModule::LuaOperateValue( uint64 objectid, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetObjectValue( uint64 objectid, const char* parentname, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, dataname );
    }

    uint64 KFLuaModule::LuaOperateObjectValue( uint64 objectid, const char* parentname, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( parentname, dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetRecordValue( uint64 objectid, const char* parentname, uint64 key, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, key, dataname );
    }

    uint64 KFLuaModule::LuaOperateRecordValue( uint64 objectid, const char* parentname, uint64 key, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( parentname, key, dataname, operate, value );
    }

    uint32 KFLuaModule::LuaGetVariable( uint32 varid )
    {
        return _run_variable[ varid ];
    }

    // 操作运行时变量
    uint32 KFLuaModule::LuaOperateVariable( uint32 varid, uint32 operate, uint32 value )
    {
        auto base = LuaGetVariable( varid );
        auto final = KFUtility::Operate( operate, base, value );
        _run_variable[ varid ] = final;

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

    void KFLuaModule::LuaAddData( uint64 objectid, const char* strelement, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( strelement, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->AddElement( &kfelements, showclient, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaSetData( uint64 objectid, const char* strelement, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( strelement, __FUNC_LINE__ );
        if ( ok )
        {
            kfelements.SetOperate( KFEnum::Set );
            kfentity->AddElement( &kfelements, showclient, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaDecData( uint64 objectid, const char* strelement )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( strelement, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->RemoveElement( &kfelements, __FUNC_LINE__ );
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
        if ( args.empty() )
        {
            return;
        }

        __JSON_PARSE_STRING__( kfjson, args );

        auto luafile = __JSON_GET_STRING__( kfjson, __KF_STRING__( luafile ) );
        auto luafunction = __JSON_GET_STRING__( kfjson, __KF_STRING__( luafunction ) );
        if ( luafile.empty() || luafunction.empty() )
        {
            return;
        }

        auto objectid = __JSON_GET_UINT64__( kfjson, __KF_STRING__( playerid ) );
        Call( luafile, luafunction, objectid, senddata, recvdata );
    }

    const char* KFLuaModule::LuaGetOptionString( const char* name, const char* logicid )
    {
        auto& strvalue = _kf_option->GetString( name, logicid );
        return strvalue.c_str();
    }

    uint32 KFLuaModule::LuaGetOptionUInt32( const char* name, const char* logicid )
    {
        return _kf_option->GetUInt32( name, logicid );
    }
}