#ifndef __KF_LUA_INTERFACE_H__
#define __KF_LUA_INTERFACE_H__

#include "KFrame.h"
#include "luaplus/LuaPlus.h"

namespace KFrame
{
    class KFScript
    {
    public:
        template< class ... T >
        void CallFunction( const char* function, uint32 objectid, T&& ...params )
        {
            try
            {
                LuaPlus::LuaObject luaobject = _lua_state->GetGlobal( function );
                if ( luaobject.IsFunction() )
                {
                    ( ( LuaPlus::LuaFunction< int32 > )luaobject )( objectid, std::forward<T>( params )... );
                }
            }
            catch ( LuaPlus::LuaException& e )
            {
                __LOG_ERROR__( KFLogEnum::Script, "call [{}] [{}] failed=[{}]", _lua_file, function, e.GetErrorMessage() );
            }
            catch ( ... )
            {
                __LOG_ERROR__( KFLogEnum::Script, "call [{}] [{}] failed unknown!", _lua_file, function );
            }
        }

    public:
        // 创建Lua解释器
        LuaPlus::LuaStateOwner _lua_state;

    protected:
        // 脚本文件
        std::string _lua_file;
    };

    class KFLuaInterface : public KFModule
    {
    public:

        // 重新加载lua文件
        virtual void LoadScript( const std::string& luafile ) = 0;
        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////

        template< class ... T >
        void CallFunction( const std::string& luafile, const std::string& function, uint32 objectid, T&& ...params )
        {
            auto kfscript = FindLuaScript( luafile );
            kfscript->CallFunction( function.c_str(), objectid, std::forward<T>( params )... );
        }

    protected:
        // 查找lua脚本
        virtual KFScript* FindLuaScript( const std::string& luafile ) = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_lua, KFLuaInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif