#include "KFLuaModule.h"
#include "KFJson.h"

namespace KFrame
{
    void KFLuaModule::BeforeRun()
    {
        _kf_player = _kf_kernel->FindComponent( __KF_STRING__( player ) );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( loadscript ), &KFLuaModule::LoadScript );
    }

    void KFLuaModule::ShutDown()
    {
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( loadscript ) );
    }

    __KF_COMMAND_FUNCTION__( KFLuaModule::LoadScript )
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
        metatableobject.RegisterObjectDirect( "LogCritical", this, &KFLuaModule::LuaLogCritical );
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
        metatableobject.RegisterObjectDirect( "STHttpClient", this, &KFLuaModule::LuaSTHttpClient );
        metatableobject.RegisterObjectDirect( "MTHttpClient", this, &KFLuaModule::LuaMTHttpClient );
        metatableobject.RegisterObjectDirect( "GetOptionString", this, &KFLuaModule::LuaGetOptionString );
        metatableobject.RegisterObjectDirect( "GetOptionUint32", this, &KFLuaModule::LuaGetOptionUint32 );

        LuaPlus::LuaObject kframeobject = kfscript->_lua_state->BoxPointer( this );
        kframeobject.SetMetatable( metatableobject );
        kfscript->_lua_state->GetGlobals().SetObject( "KFrame", kframeobject );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    void KFLuaModule::LuaLogCritical( const char* data )
    {
        __LOG_CRITICAL__( "{}", data );
    }

    const char* KFLuaModule::LuaMd5Encode( const char* data )
    {
        static std::string _result;
        _result = KFCrypto::Md5Encode( data );
        return _result.c_str();
    }

    uint64 KFLuaModule::LuaGetValue( uint32 objectid, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( dataname );
    }

    uint64 KFLuaModule::LuaOperateValue( uint32 objectid, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetObjectValue( uint32 objectid, const char* parentname, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, dataname );
    }

    uint64 KFLuaModule::LuaOperateObjectValue( uint32 objectid, const char* parentname, const char* dataname, const char* operate, const char* value )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto uoperate = KFUtility::ToValue< uint32 >( operate );
        auto uvalue = KFUtility::ToValue< uint64 >( value );
        return kfentity->UpdateData( parentname, dataname, uoperate, uvalue );
    }

    uint64 KFLuaModule::LuaGetRecordValue( uint32 objectid, const char* parentname, uint64 key, const char* dataname )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, key, dataname );
    }

    uint64 KFLuaModule::LuaOperateRecordValue( uint32 objectid, const char* parentname, uint64 key, const char* dataname, uint32 operate, uint64 value )
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

    uint32 KFLuaModule::LuaGetRealTime()
    {
        return KFGlobal::Instance()->_real_time;
    }

    void KFLuaModule::LuaAddData( uint32 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->AddAgentData( &kfagents, 1.0f, showclient, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaSetData( uint32 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfagents.SetOperate( KFOperateEnum::Set );
            kfentity->AddAgentData( &kfagents, 1.0f, showclient, __FUNC_LINE__ );
        }
    }

    void KFLuaModule::LuaDecData( uint32 objectid, const char* stragent )
    {
        auto kfentity = _kf_player->FindEntity( objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNC_LINE__ );
        if ( ok )
        {
            kfentity->RemoveAgentData( &kfagents, __FUNC_LINE__ );
        }
    }

    const char* KFLuaModule::LuaSTHttpClient( const char* url, const char* data )
    {
        static std::string _result = "";
        _result = _kf_http_client->StartSTHttpClient( url, data );
        return _result.c_str();
    }

    void KFLuaModule::LuaMTHttpClient( const char* url, const char* data, const char* callback )
    {
        _kf_http_client->StartMTHttpClient( this, &KFLuaModule::OnLuaHttpCallBack, url, data, callback );
    }

    __KF_HTTP_CALL_BACK_FUNCTION__( KFLuaModule::OnLuaHttpCallBack )
    {
        KFJson kfjson( callback );

        auto objectid = kfjson.GetUInt32( __KF_STRING__( playerid ) );
        auto luafile = kfjson.GetString( __KF_STRING__( luafile ) );
        auto luafunction = kfjson.GetString( __KF_STRING__( luafunction ) );
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