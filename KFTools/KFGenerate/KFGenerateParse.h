#ifndef __KF_GENERATE_PARSE_H__
#define __KF_GENERATE_PARSE_H__

#include "KFGenerateDefine.h"
#include "KFExcel/KFExcelFile.h"

namespace KFrame
{
    class KFGenerateParse : public KFSingleton< KFGenerateParse >
    {
    public:
        // 解析文件列表
        void ParseExcels( StringList& listfile );

        // 解析excel文件
        bool ParseExcel( std::string& file );

        // 获得错误信息
        const std::string GetError() const
        {
            return _error;
        }

    protected:
        // 解析文件线程
        void ParseExecelThread();

        // 加载excel文件
        bool LoadExcel( KFExcelSheet* sheet, const std::string& version );

        // 读取option数据
        void ReadExcelOptionData( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 读取字段属性
        bool ReadExcelAttribute( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 读取配置表数据
        bool ReadExcelContent( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 保存配置文件
        bool WriteConfigFile( ExcelFileData* exceldata, const std::string& version );

        // 保存xml文件
        bool WriteXmlFile( ExcelFileData* exceldata, std::string filepath, std::string filename, const std::string& version );

        // 生成代码
        bool WriteCodeFile( ExcelFileData* exceldata );

        // 生成cpp文件
        bool GenreateCppFile( ExcelFileData* exceldata, std::string filename );
        // 检查cpp类数据
        void CheckCppClassData( ExcelFileData* exceldata, std::map< std::string, CodeClass >& classlist );
        // 获得cpp父类列表
        void FormatCppParentClass( ExcelAttribute* attribute, std::map< std::string, CodeClass >& classlist );
        // 写文件
        bool WriteCppFile( ExcelFileData* exceldata, std::map< std::string, CodeClass >& classlist, std::string filename );
        // 判断是否需要写加载完成函数
        bool IsNeedWriteLoadAllComplete( ExcelFileData* exceldata );

    private:
        // 把配置表字段名转换成cpp的变量名
        std::string TransformNameToCpp( const std::string& name );

        // 格式化子类变量
        std::string FormatParentVariable( const std::string& classname );

        // 判断子类的类型
        bool IsParentCppClass( ExcelFileData* exceldata, const std::string& parentclass, const std::string& cppclass );

        // 判断是否需要写cpp文件
        bool IsNeedWriteCppVariable( const ExcelAttribute* attribute, const std::string& keyname );

        // 获得Cpp Parent类的数据(是否要写类定义, 注释, 变量)
        std::tuple<uint32, std::string, std::string> FormatParentOption( ExcelFileData* exceldata, const std::string& classname );

        // 删除KF前缀
        std::string RemovePrefix( std::string name, const std::string& prefix );

    private:
        // 错误信息
        std::string _error;

        // 文件列表
        StringList _file_list;

        // 线程锁
        KFMutex _kf_mutex;

        // 完成数量
        std::atomic<uint32> _finish_count;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _parse = KFrame::KFGenerateParse::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
