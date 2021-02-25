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
        uint64 CallFunction( const std::string& function, uint64 object_id, T&& ...params )
        {
            try
            {
                LuaPlus::LuaObject lua_object = _lua_state->GetGlobal( function.c_str() );
                if ( lua_object.IsFunction() )
                {
                    auto result = ( ( LuaPlus::LuaFunction<uint64> )lua_object )( object_id, std::forward<T>( params )... );
                    return result;
                }
            }
            catch ( LuaPlus::LuaException& e )
            {
                __LOG_ERROR__( "call [{}] [{}] failed=[{}]", _lua_file, function, e.GetErrorMessage() );
            }
            catch ( ... )
            {
                __LOG_ERROR__( "call [{}] [{}] failed unknown", _lua_file, function );
            }

            return 0u;
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
