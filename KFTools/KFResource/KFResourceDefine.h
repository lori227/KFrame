#ifndef __KF_RESOURCE_DEFINE_H__
#define __KF_RESOURCE_DEFINE_H__

#include "KFrame.h"
#include "stdafx.h"
using namespace KFrame;

namespace KFResource
{
    static const std::string _setting_file = "_zdata.xml";
    static const std::string _resource_file = "_zresource.xml";

    namespace FileType
    {
        enum MyEnum
        {
            None = 0x0,		// 无效
            Server = 0x1,	// 服务器
            Client = 0x2,	// 客户端
            All = 0x3,		// 公共文件
        };
    };

    static const char* _file_type_name[] =
    {
        "无效文件",
        "服务器文件",
        "客户端文件",
        "公用文件",
    };

    static const char* _store_name[] =
    {
        "svn",
        "git",
    };
    static uint32 _store_count = std::extent<decltype( _store_name )>::value;
}

#endif
