#ifndef __KF_GENERATE_LOGIC_H__
#define __KF_GENERATE_LOGIC_H__

#include "KFGenerateDefine.h"
#include "KFGenerateEvent.h"

namespace KFrame
{
    class KFGenerateLogic : public KFSingleton< KFGenerateLogic >
    {
    public:
        // 加载临时数据
        bool LoadTempXml();
        // 保存临时数据
        bool SaveTempXml();

        // 加载生成配置数据
        bool LoadGenerateXml();

        // 加载配置表数据
        bool LoadExcelXml();
        // 保存配置表数据
        bool SaveExcelXml();

    public:
        // 检查文件md5线程
        void RunCheckExecelMd5Thread();


    protected:
        // 检查所有的文件
        void CheckAllExecelMd5();

        // 判断是否Excel文件
        bool IsExcelFile( const std::string& path );

    public:
        // 项目名称
        std::string _project_name;

        // 项目时间
        uint64 _project_time = 0;

        // 服务器配置表存放路径
        std::string _server_xml_path;

        // 客户端配置表存放路径
        std::string _client_xml_path;

        // cpp文件存放路径
        std::string _cpp_file_path;

        // c#文件存放路径
        std::string _csharp_file_path;

        // lua 文件存放路径
        std::string _lua_file_path;

        // 生成文件类型( FileType )
        uint32 _file_type = 0u;

        // 当前选择的仓库类型
        std::string _repository_type;
        // 仓库数据列表
        KFHashMap< std::string, const std::string&, RepositoryData > _repository_list;

        // 服务器列表
        std::string _server_id;
        KFHashMap< std::string, const std::string, ServerData > _server_list;

        // 类型列表
        KFHashMap< std::string, const std::string&, TypeInfo > _type_list;

        // 文件列表
        KFMap< std::string, const std::string&, FileData > _file_list;

    public:
        // 逻辑线程run
        bool _thread_run = true;

        // 事件锁
        KFMutex _mutex;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _logic = KFrame::KFGenerateLogic::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
