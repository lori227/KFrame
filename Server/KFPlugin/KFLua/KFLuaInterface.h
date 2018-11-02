#ifndef __KF_LUA_INTERFACE_H__
#define __KF_LUA_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
    class KFLuaInterface : public KFModule
    {
    public:
        // 调用lua函数, uint64参数
        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1, uint64 param2 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1, uint64 param2, uint64 param3 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1, uint64 param2, uint64 param3, uint64 param4 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           uint64 param1, uint64 param2, uint64 param3, uint64 param4, uint64 param5, uint64 param6 ) = 0;


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 调用lua函数, string参数
        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1, const std::string& param2 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1, const std::string& param2, const std::string& param3 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1, const std::string& param2, const std::string& param3,
                           const std::string& param4 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1, const std::string& param2, const std::string& param3,
                           const std::string& param4, const std::string& param5 ) = 0;

        virtual void Call( const std::string& luafile, const std::string& function, uint64 objectid,
                           const std::string& param1, const std::string& param2, const std::string& param3,
                           const std::string& param4, const std::string& param5, const std::string& param6 ) = 0;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_INTERFACE__( _kf_lua, KFLuaInterface );
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif