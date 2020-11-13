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
        bool SaveConfigFile( ExcelFileData* exceldata );

        // 保存xml文件
        bool SaveXmlFile( ExcelFileData* exceldata, std::string filename );

    private:

        // 错误信息
        std::string _error;
    };
}
///////////////////////////////////////////////////////////////////////////////////////////////
static auto _parse = KFrame::KFGenerateParse::Instance();
///////////////////////////////////////////////////////////////////////////////////////////////

#endif
