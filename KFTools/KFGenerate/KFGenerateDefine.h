#ifndef __KF_GENERATE_DEFINE_H__
#define __KF_GENERATE_DEFINE_H__

#include "KFrame.h"

namespace KFrame
{
    static const std::string _temp_xml = "_temp.xml";			// 生成路径配置( 每个机器都不通 )
    static const std::string _excel_xml = "_excel.xml";			// 文件列表配置( 由程序生成 )
    static const std::string _generate_xml = "_generate.xml";	// 仓库, 服务器配置( 不变 )

    static const std::string _str_cpp = "cpp";
    static const std::string _str_xml = "xml";
    static const std::string _str_key = "key";
    static const std::string _str_repeat = "repeat";
    static const std::string _str_vector = "vector";
    static const std::string _str_list = "list";
    static const std::string _str_set = "set";
    static const std::string _str_map = "map";
    static const std::string _str_weight = "KFWeightList";

    static const std::string _str_kf_prefix = "KF";
    static const std::string _str_element = "element";
    static const std::string _str_execute = "execute";
    static const std::string _str_condition_define = "conditiondefine";
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
        // cpp读取函数
        std::string _cpp_function;
        // cpp变量名读物额外变量
        std::string _cpp_extend;

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
        // 路径
        std::string _path;

        // 文件名
        std::string _name;

        // 文件类型( FileType 枚举 )
        uint32 _type = FileType::All;

        // 修改时间
        uint64 _time;

        // md5值
        std::string _md5_current;

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
            ShowMessage = 0,
            RemoveFile = 1,
            AddFile = 2,
            ParseOk = 3,
            ParseFailed = 4,
            ParseFinish = 5,
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 生成选项
    class ExcelOption
    {
    public:
        bool IsValid() const
        {
            return !_key.empty() && !_value.empty();
        }
    public:
        // 选项名
        std::string _key;

        // 选项值
        std::string _value;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 属性的描述
    class ExcelAttribute
    {
    public:
        // 列索引
        uint32 _column = 0u;

        // 字段名
        std::string _name;

        // xml字段
        std::string _config_name;

        // cpp名字
        std::string _cpp_name;

        // 类型名
        std::string _cpp_class;
        std::string _cpp_parent_class;

        std::string _csharp_class;

        // 生成类型( FileType )
        uint32 _flag = 0u;

        // 注释
        std::string _comments;

        // 是否必须填写字段
        std::string _optional = "true";
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 属性的一行
    class ExcelRow
    {
    public:
        void AddData( uint32 index, const std::string& value )
        {
            _columns[ index ] = value;
        }

        bool IsValid()
        {
            for ( auto& iter : _columns )
            {
                if ( !iter.second.empty() )
                {
                    return true;
                }
            }

            return false;
        }

    public:
        // 行数
        uint32 _row = 0u;

        // 数据列表
        UInt32String _columns;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Excel文件属性
    class ExcelFileData
    {
    public:
        const ExcelAttribute* FindAttribute( uint32 column )
        {
            auto iter = _attributes.find( column );
            if ( iter == _attributes.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }

        const ExcelOption* FindOption( const std::string& key )
        {
            auto iter = _options.find( key );
            if ( iter == _options.end() )
            {
                return nullptr;
            }

            return &iter->second;
        }

        bool HaveCppAttribute( const std::string& cppclass )
        {
            for ( auto& iter : _attributes )
            {
                if ( iter.second._cpp_class == cppclass )
                {
                    return true;
                }
            }

            return false;
        }

        bool IsRepeat() const
        {
            auto iter = _options.find( _str_repeat );
            if ( iter == _options.end() )
            {
                return false;
            }

            return iter->second._value == "1";
        }

    public:
        // 选项列表
        std::map< std::string, ExcelOption > _options;

        // 描述信息
        std::map< uint32, ExcelAttribute > _attributes;

        // 数据信息
        std::vector< ExcelRow > _datas;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    namespace CodeEnum
    {
        enum MyEnum
        {
            None = 0,
            WriteClass = 1,		// 需要写子类
            WriteNothing = 2,	// 不需要写子类
        };
    }

    // 类结构
    class CodeClass
    {
    public:
        // 类名
        std::string _name;

        // 属性字段(列索引)
        std::vector< uint32 > _columns;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif
