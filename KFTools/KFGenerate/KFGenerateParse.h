#ifndef __KF_GENERATE_PARSE_H__
#define __KF_GENERATE_PARSE_H__

#include "KFGenerateDefine.h"
#include "KFExcel/KFExcelFile.h"

namespace KFrame
{
    class KFGenerateParse : public KFSingleton< KFGenerateParse >
    {
    public:
        // 加载excel文件
        bool ParseExcel( const char* file );

        // 获得错误信息
        const std::string GetError() const
        {
            return _error;
        }

    protected:
        // 加载excel文件
        bool LoadExcel( KFExcelSheet* sheet );

        // 读取option数据
        void ReadExcelOptionData( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 读取字段属性
        bool ReadExcelAttribute( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 读取配置表数据
        bool ReadExcelContent( KFExcelSheet* sheet, ExcelFileData* exceldata );

        // 保存配置文件
        bool WriteConfigFile( ExcelFileData* exceldata );

        // 保存xml文件
        bool WriteXmlFile( ExcelFileData* exceldata, std::string filename );

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



    private:
        // 把配置表字段名转换成cpp的变量名
        std::string TransformNameToCpp( const std::string& name );

    private:

        // 错误信息
        std::string _error;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _parse = KFrame::KFGenerateParse::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
