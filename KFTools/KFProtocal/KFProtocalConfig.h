#ifndef __KF_PROTOCAL_CONFIG_H__
#define __KF_PROTOCAL_CONFIG_H__

#include "KFrame.h"
#include "KFSingleton.h"
#include "stdafx.h"

namespace KFrame
{
    class KFProtocalConfig : public KFSingleton< KFProtocalConfig >
    {
    public:

        // 加载配置
        bool LoadFile( const std::string& file );

        // 保存配置
        bool SaveFile( const std::string& file );

    public:

        static const std::string _setting_file;

        // 服务器cpp路径
        std::string _server_cpp_path;

        // 服务器lua路径
        std::string _server_lua_path;

        // 客户端cpp路径
        std::string _client_cpp_path;

        // 客户端lua路径
        std::string _client_lua_path;

    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _protocal_config = KFrame::KFProtocalConfig::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
