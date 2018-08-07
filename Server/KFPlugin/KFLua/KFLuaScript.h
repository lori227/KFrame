#ifndef __KF_LUA_SCRIPT_H__
#define __KF_LUA_SCRIPT_H__

#include "KFrame.h"
#include "KFLuaInterface.h"
#include "KFLogClient/KFLogClientInterface.h"

namespace KFrame
{
    class KFLuaScript : public KFScript
    {
    public:
        KFLuaScript();
        ~KFLuaScript();

        // 加载脚本函数
        void LoadScript( const std::string& file );

        // 释放脚本
        void ReleaseScript();

    public:
        // 是否初始化函数
        bool _is_initialized;
    };
}

#endif
