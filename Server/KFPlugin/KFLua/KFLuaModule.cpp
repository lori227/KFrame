#include "KFLuaModule.h"
#include "KFJson.h"

namespace KFrame
{
    void KFLuaModule::BeforeRun()
    {
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
    KFScript* KFLuaModule::FindLuaScript( const std::string& luafile )
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFLuaModule::RegisterLuaFunction( KFLuaScript* kfscript )
    {
        auto metatableobject = kfscript->_lua_state->GetGlobals().CreateTable( "MultiObjectMetaTable" );
        metatableobject.SetObject( "__index", metatableobject );

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

        LuaPlus::LuaObject kframeobject = kfscript->_lua_state->BoxPointer( this );
        kframeobject.SetMetatable( metatableobject );
        kfscript->_lua_state->GetGlobals().SetObject( "KFrame", kframeobject );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFLuaModule::LuaGetValue( const char* module, uint32 objectid, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( dataname );
    }

    uint64 KFLuaModule::LuaOperateValue( const char* module, uint32 objectid, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, dataname );
    }

    uint64 KFLuaModule::LuaOperateObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname, const char* operate, const char* value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto uoperate = KFUtility::ToValue< uint32 >( operate );
        auto uvalue = KFUtility::ToValue< uint64 >( value );
        return kfentity->UpdateData( parentname, dataname, uoperate, uvalue );
    }

    uint64 KFLuaModule::LuaGetRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, key, dataname );
    }

    uint64 KFLuaModule::LuaOperateRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
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

    void KFLuaModule::LuaAddData( const char* module, uint32 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
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

    void KFLuaModule::LuaSetData( const char* module, uint32 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
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

    void KFLuaModule::LuaDecData( const char* module, uint32 objectid, const char* stragent )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNC_LINE__ );
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

    void KFLuaModule::LuaMTHttpClient( const char* url, const char* data )
    {
        _kf_http_client->StartMTHttpClient( url, data, this, &KFLuaModule::OnLuaHttpCallBack );
    }

    void KFLuaModule::OnLuaHttpCallBack( std::string& senddata, std::string& recvdata )
    {
        KFJson kfjson( senddata );

        auto objectid = kfjson.GetUInt32( __KF_STRING__( playerid ) );
        auto luafile = kfjson.GetString( __KF_STRING__( luafile ) );
        auto luafunction = kfjson.GetString( __KF_STRING__( luafunction ) );
        if ( luafile.empty() || !luafunction.empty() )
        {
            return;
        }

        CallFunction( luafile, luafunction, objectid, senddata.c_str(), recvdata.c_str() );
    }
}