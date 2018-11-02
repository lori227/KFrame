#ifndef __KF_LUA_SCRIPT_H__
#define __KF_LUA_SCRIPT_H__

#include "KFrame.h"
#include "luaplus/LuaPlus.h"

namespace KFrame
{
    class KFLuaScript
    {
    public:
        KFLuaScript();
        ~KFLuaScript();

        // 加载脚本函数
        void LoadScript( const std::string& file );

        // 释放脚本
        void ReleaseScript();

        template< class ... T >
        void CallFunction( const std::string& function, uint32 objectid, T&& ...params )
        {
            try
            {
                LuaPlus::LuaObject luaobject = _lua_state->GetGlobal( function.c_str() );
                if ( luaobject.IsFunction() )
                {
                    ( ( LuaPlus::LuaFunction< int32 > )luaobject )( objectid, std::forward<T>( params )... );
                }
            }
            catch ( LuaPlus::LuaException& e )
            {
                __LOG_ERROR__( "call [{}] [{}] failed=[{}]", _lua_file, function, e.GetErrorMessage() );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "call [{}] [{}] failed unknown!", _lua_file, function );
            }
        }

    public:
        // 是否初始化函数
        bool _is_initialized;

        // 创建Lua解释器
        LuaPlus::LuaStateOwner _lua_state;

    protected:
        // 脚本文件
        std::string _lua_file;
    };
}

#endif
