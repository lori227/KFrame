#ifndef __KF_LUA_MODULE_H__
#define __KF_LUA_MODULE_H__

/************************************************************************
//    @Module			:    Lua模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-1-22
************************************************************************/

#include "KFrame.h"
#include "KFLuaScript.h"
#include "KFLuaInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFDeployCommand/KFDeployCommandInterface.h"

namespace KFrame
{
    class KFLuaModule : public KFLuaInterface
    {
    public:
        KFLuaModule() = default;
        ~KFLuaModule() = default;

        // 初始化
        virtual void BeforeRun();

        // 关闭
        virtual void ShutDown();
        ////////////////////////////////////////////////////////////////////////////////
        // 重新加载脚本文件
        __KF_COMMAND_FUNCTION__( LoadScript );
        ////////////////////////////////////////////////////////////////////////////////

        // 获得基础属性
        uint64 LuaGetValue( const char* module, uint32 objectid, const char* dataname );

        // 操作属性
        uint64 LuaOperateValue( const char* module, uint32 objectid, const char* dataname, uint32 operate, uint64 value );

        // 获得对象属性
        uint64 LuaGetObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname );

        // 操作对象属性
        uint64 LuaOperateObjectValue( const char* module, uint32 objectid, const char* parentname, const char* dataname, const char* operate, const char* value );

        // 获得数组属性
        uint64 LuaGetRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname );

        // 操作数组属性
        uint64 LuaOperateRecordValue( const char* module, uint32 objectid, const char* parentname, uint64 key, const char* dataname, uint32 operate, uint64 value );

        // 获得运行时变量
        uint32 LuaGetVariable( uint32 varid );

        // 操作运行时变量
        uint32 LuaOperateVariable( uint32 varid, uint32 operate, uint32 value );

        // 获得游戏时间
        uint64 LuaGetGameTime();

        // 获得现实时间
        uint64 LuaGetRealTime();

        // 添加数据
        void LuaAddData( const char* module, uint32 objectid, const char* stragent, bool showclient );

        // 设置数据
        void LuaSetData( const char* module, uint32 objectid, const char* stragent, bool showclient );

        // 减少数据
        void LuaDecData( const char* module, uint32 objectid, const char* stragent );

        // http访问
        const char* LuaSTHttpClient( const char* url, const char* data );
        void LuaMTHttpClient( const char* url, const char* data );

    protected:
        ////////////////////////////////////////////////////////////////////////////////
        // 查找lua脚本
        virtual KFScript* FindLuaScript( const std::string& luafile );

        // 注册lua导出函数
        void RegisterLuaFunction( KFLuaScript* luascript );

        // lua http回调函数
        void OnLuaHttpCallBack( std::string& senddata, std::string& recvdata );
    private:
        // lua 脚本
        KFMap< std::string, const std::string&, KFLuaScript > _lua_script;

        // 运行时的一些变量
        std::map< uint32, uint32 > _run_variable;
    };
}



#endif