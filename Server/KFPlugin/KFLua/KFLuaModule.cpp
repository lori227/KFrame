#include "KFLuaModule.h"

namespace KFrame
{
    KFLuaModule::KFLuaModule()
    {
    }

    KFLuaModule::~KFLuaModule()
    {

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

    void KFLuaModule::LoadScript( const std::string& luafile )
    {
        auto kfluascript = _lua_script.Find( luafile );

        if ( kfluascript == nullptr )
        {
            return;
        }

        kfluascript->LoadScript( luafile );
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
        metatableobject.RegisterObjectDirect( "AddRecent", this, &KFLuaModule::LuaAddRecent );

        LuaPlus::LuaObject kframeobject = kfscript->_lua_state->BoxPointer( this );
        kframeobject.SetMetatable( metatableobject );
        kfscript->_lua_state->GetGlobals().SetObject( "KFrame", kframeobject );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFLuaModule::LuaGetValue( const char* module, uint32 objectid, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( dataname );
    }

    uint64 KFLuaModule::LuaOperateValue( const char* module, uint32 objectid, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, dataname );
    }

    uint64 KFLuaModule::LuaOperateObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        return kfentity->UpdateData( parentname, dataname, operate, value );
    }

    uint64 KFLuaModule::LuaGetRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return _invalid_int;
        }

        auto kfobject = kfentity->GetData();
        return kfobject->GetValue( parentname, key, dataname );
    }

    uint64 KFLuaModule::LuaOperateRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname, uint32 operate, uint64 value )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

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
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNCTION_LINE__ );

        if ( ok )
        {
            kfentity->AddAgentData( &kfagents, 1.0f, showclient, __FUNCTION_LINE__ );
        }
    }

    void KFLuaModule::LuaSetData( const char* module, uint32 objectid, const char* stragent, bool showclient )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNCTION_LINE__ );

        if ( ok )
        {
            kfagents.SetOperate( KFOperateEnum::Set );
            kfentity->AddAgentData( &kfagents, 1.0f, showclient, __FUNCTION_LINE__ );
        }
    }

    void KFLuaModule::LuaDecData( const char* module, uint32 objectid, const char* stragent )
    {
        auto kfentity = _kf_kernel->FindEntity( module, objectid, __FUNCTION_LINE__ );

        if ( kfentity == nullptr )
        {
            return;
        }

        KFAgents kfagents;
        auto ok = kfagents.ParseFromString( stragent, __FUNCTION_LINE__ );

        if ( ok )
        {
            kfentity->RemoveAgentData( &kfagents, __FUNCTION_LINE__ );
        }
    }

    void KFLuaModule::LuaAddRecent( const char* module, uint32 objectid, const char* stragent )
    {
        KFMsg::S2SAddBattleFriendDataReq req;

        {
            auto pbrecentdata = req.add_recentdata();
            pbrecentdata->set_playerid( KFUtility::ToValue<uint32>( stragent ) );
            pbrecentdata->set_ranking( 1 );
            pbrecentdata->set_totalnum( 100 );
            pbrecentdata->set_kill( 10 );
            pbrecentdata->set_score( 2 );
        }

        {
            auto pbrecentdata = req.add_recentdata();
            pbrecentdata->set_playerid( objectid );
            pbrecentdata->set_ranking( 1 );
            pbrecentdata->set_totalnum( 100 );
            pbrecentdata->set_kill( 10 );
            pbrecentdata->set_score( 2 );
        }

        _kf_relation->SendMessageToRelation( KFMsg::S2S_ADD_BATTLE_FRIEND_DATA_REQ, &req );
    }

}