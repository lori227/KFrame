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
        bool LoadFromExcel( const char* file, bool utf8, uint32 save_flag );
        bool SaveToHpp( const char* file );
        bool SaveToCpp( const char* file );

        bool SaveToXml( const char* path );
        bool SaveToCSV( const char* path );


        // 保存到c#文件
        bool SaveToCSharp( const char* path );

        //在每个大写字母前加下划线，把大写字母转为小写字母
        std::string changeString( std::string& oldstring );

    private:
        bool LoadFromExcel( KFExcelSheet* sheet, uint32 save_flag );

        KFFile* AddFile( std::string& name );

        bool SaveToHpp( const std::string& path, KFFile* kffile );
        bool SaveToCpp( const std::string& path, KFFile* kffile );

        bool SaveToXml( const std::string& path, KFFile* kffile );
        bool SaveToCSV( const std::string& path, KFFile* kffile );

        bool SaveToCSharp( const std::string& path, KFFile* kffile );

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
