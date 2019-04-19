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
        auto kfluascript = _lua_script.Find( param );
        if ( kfluascript == nullptr )
        {
            return;
        }

        kfluascript->LoadScript( param );
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
    void KFLuaModule::RegisterLuaFunction( KFLuaScript* kfscript )
    {
        auto metatableobject = kfscript->_lua_state->GetGlobals().CreateTable( "MultiObjectMetaTable" );
        metatableobject.SetObject( "__index", metatableobject );

        metatableobject.RegisterObjectDirect( "LogDebug", this, &KFLuaModule::LuaLogDebug );
        metatableobject.RegisterObjectDirect( "LogInfo", this, &KFLuaModule::LuaLogInfo );
        metatableobject.RegisterObjectDirect( "LogWarn", this, &KFLuaModule::LuaLogWarn );
        metatableobject.RegisterObjectDirect( "LogError", this, &KFLuaModule::LuaLogError );
        metatableobject.RegisterObjectDirect( "Md5Encode", this, &KFLuaModule::LuaMd5Encode );
        metatableobject.RegisterObjectDirect( "GetValue", this, &KFLuaModule::LuaGetValue );
        metatableobject.RegisterObjectDirect( "OperateValue", this, &KFLuaModule::LuaOperateValue );
        metatableobject.RegisterObjectDirect( "GetObjectValue", this, &KFLuaModule::LuaGetObjectValue );
        metatableobject.RegisterObjectDirect( "OperateObjectValue", this, &KFLuaModule::LuaOperateObjectValue );
        metatableobject.RegisterObjectDirect( "GetRecordValue", this, &KFLuaModule::LuaGetRecordValue );
        metatableobject.RegisterObjectDirect( "OperateRecordValue", this, &KFLuaModule::LuaOperateRecordValue );
        metatableobject.RegisterObjectDirect( "GetVariable", this, &KFLuaModule::LuaGetVariable );
        metatableobject.RegisterObjectDirect( "OperateVariable", this, &KFLuaModule::LuaOperateVariable );
        metatableobject.RegisterObjectDirect( "GetGameTime", this, &KFLuaModule::LuaGetGameTime );
        metatableobject.RegisterObjectDirect( "GetRealTime", this, &KFLuaModule::LuaGetRealTime );
        metatableobject.RegisterObjectDirect( "AddData", this, &KFLuaModule::LuaAddData );
        metatableobject.RegisterObjectDirect( "SetData", this, &KFLuaModule::LuaSetData );
        metatableobject.RegisterObjectDirect( "DecData", this, &KFLuaModule::LuaDecData );
        metatableobject.RegisterObjectDirect( "STHttpGet", this, &KFLuaModule::LuaSTGet );
        metatableobject.RegisterObjectDirect( "STHttpPost", this, &KFLuaModule::LuaSTPost );
        metatableobject.RegisterObjectDirect( "MTHttpGet", this, &KFLuaModule::LuaMTGet );
        metatableobject.RegisterObjectDirect( "MTHttpPost", this, &KFLuaModule::LuaMTPost );
        metatableobject.RegisterObjectDirect( "GetOptionString", this, &KFLuaModule::LuaGetOptionString );
        metatableobject.RegisterObjectDirect( "GetOptionUint32", this, &KFLuaModule::LuaGetOptionUint32 );

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

    void KFLuaModule::LuaAddData( uint64 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->AddElement( __FUNC_LINE__, &kfelements, showclient );
        }
    }

    void KFLuaModule::LuaSetData( uint64 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfelements.SetOperate( KFEnum::Set );
            kfentity->AddElement( __FUNC_LINE__, &kfelements, showclient );
        }
    }

    void KFLuaModule::LuaDecData( uint64 objectid, const char* stragent )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFElements kfelements;
        auto ok = kfelements.Parse( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->RemoveElement( __FUNC_LINE__, &kfelements );
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
        __JSON_PARSE_STRING__( kfjson, args );

        auto objectid = __JSON_GET_UINT64__( kfjson, __KF_STRING__( playerid ) );
        auto luafile = __JSON_GET_STRING__( kfjson, __KF_STRING__( luafile ) );
        auto luafunction = __JSON_GET_STRING__( kfjson, __KF_STRING__( luafunction ) );
        if ( luafile.empty() || luafunction.empty() )
        {
            return;
        }

        Call( luafile, luafunction, objectid, senddata, recvdata );
    }

    const char* KFLuaModule::LuaGetOptionString( const char* name, const char* logicid )
    {
        auto& strvalue = _kf_option->GetString( name, logicid );
        return strvalue.c_str();
    }

    uint32 KFLuaModule::LuaGetOptionUint32( const char* name, const char* logicid )
    {
        return _kf_option->GetUInt32( name, logicid );
    }
}