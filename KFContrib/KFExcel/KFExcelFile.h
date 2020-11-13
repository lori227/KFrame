#ifndef __KF_EXCEL_FILE_H__
#define __KF_EXCEL_FILE_H__

#include "KFExcelSheet.h"

namespace KFrame
{
    class KFExcelZip;
    class KFExcelFile
    {
    public:
        KFExcelFile();
        ~KFExcelFile();

        bool Open( const char* file );
        KFExcelSheet* FindSheet( const char* name );

    private:

        void ReadWorkBook( const char* file );
        void ReadWorkBookRels( const char* file );
        void ReadSharedStrings( const char* file );
        void ReadStyles( const char* file );
        void ReadSheet( KFExcelSheet* sheet );

        void ParseCell( const std::string& value, int& row, int& col );
        void ParseRange( const std::string& value, KFExcelRange& range );

        void ReplaceString( std::string& srcstring, const std::string& strold, const std::string& strnew );
    public:
        KFExcelZip* _zip;

        StringVector _shared_string;
        std::vector<KFExcelSheet*> _sheets;
    };
}


#endif
