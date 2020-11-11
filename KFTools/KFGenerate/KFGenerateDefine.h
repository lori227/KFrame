#ifndef __KF_GENERATE_DEFINE_H__
#define __KF_GENERATE_DEFINE_H__

#include "KFrame.h"

namespace KFrame
{
    static const std::string _temp_xml = "_temp.xml";			// 生成路径配置( 每个机器都不通 )
    static const std::string _excel_xml = "_excel.xml";			// 文件列表配置( 由程序生成 )
    static const std::string _generate_xml = "_generate.xml";	// 仓库, 服务器配置( 不变 )
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 仓库数据
    class RepositoryData
    {
    public:
        // 类型
        std::string _type;

        // 账号
        std::string _user;

        // 密码
        std::string _password;

        // 路径
        std::string _path;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 部署中控服务器地址
    class ServerData
    {
    public:
        // id
        std::string _id;

        // 账号
        std::string _name;

        // 端口
        std::string _url;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 配置表类型对应的代码类型
    class TypeInfo
    {
    public:
        // 配置表名称
        std::string _name;

        // cpp类型
        std::string _cpp_name;

        // csharp类型
        std::string _csharp_name;

        // lua类型
        std::string _lua_name;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    class FileData
    {
    public:
        // 文件名
        std::string _name;

        // 文件类型( FileType 枚举 )
        uint32 _type = FileType::All;

        // 修改时间
        uint64 _time;

        // md5值
        std::string _md5_server_current;
        std::string _md5_client_current;

        // md5值
        std::string _md5_client_repository;
        std::string _md5_server_repository;

        // 生成的配置文件md5值
        std::map<std::string, std::string> _md5_list;

        // 上次文件检查时间
        uint64 _last_md5_check_time = 0u;

    public:
        // 服务器文件
        bool IsServerFile()
        {
            return KFUtility::HaveBitMask( _type, ( uint32 )FileType::Server );
        }

        // 客户端文件
        bool IsClientFile()
        {
            return KFUtility::HaveBitMask( _type, ( uint32 )FileType::Client );
        }
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace EventType
    {
        enum MyEnum
        {
            RemoveFile = 1,
            AddFile = 2,
        };
    }

    class EventData
    {
    public:
        // 事件类型
        uint32 _type = 0u;

        // 事件参数
        uint32 _int_param = 0u;

        // 事件参数
        std::string _str_param;
    };
}

#endif
