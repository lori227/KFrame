#ifndef __KF_PARSE_H__
#define __KF_PARSE_H__

#include "KFSingleton.h"
#include "KFInterfaces.h"
#include "KFExcel/KFExcelFile.h"
#include "KFFile.h"

namespace KFrame
{
    class KFParse : public KFSingleton< KFParse >
    {
    public:
        bool LoadFromExcel( const char* file, bool utf8, uint32 saveflag );

        std::string SaveToXml( const char* path, const std::string& version );
        bool SaveToCSV( const char* path, const std::string& version );

    private:
        bool LoadFromExcel( KFExcelSheet* sheet, uint32 saveflag );

        KFFile* AddFile( std::string& name );

        std::string SaveToXml( const std::string& path, KFFile* kffile, const std::string& version );
        bool SaveToCSV( const std::string& path, KFFile* kffile, const std::string& version );

    public:
        // error
        std::string _error;

    private:
        bool _utf8 = false;

        // 文件
        std::unordered_map< std::string, KFFile > _files;
    };

    ///////////////////////////////////////////////////////////////////////////
    static auto _kf_parse = KFParse::Instance();
    ///////////////////////////////////////////////////////////////////////////
}
#endif
