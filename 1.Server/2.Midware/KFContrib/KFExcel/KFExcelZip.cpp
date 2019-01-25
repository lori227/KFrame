#include "KFExcelZip.hpp"
#include "xml/rapidxml_print.hpp"
#include "xml/rapidxml_utils.hpp"

namespace KFrame
{
    KFExcelZip::KFExcelZip()
    {

    }

    KFExcelZip::~KFExcelZip()
    {
        unzClose( _zipFile );
    }

    bool KFExcelZip::Open( const char* file )
    {
        _zipFile = unzOpen( file );

        if ( !_zipFile )
            return false;

        char szCurrentFileName[ 260 ] = { 0 };
        unz_file_info64 fileInfo;

        int err = unzGoToNextFile2( _zipFile, &fileInfo,
                                    szCurrentFileName, sizeof( szCurrentFileName ) - 1, nullptr, 0, nullptr, 0 );
        while ( err == UNZ_OK )
        {
            unz_file_pos posInfo;
            if ( unzGetFilePos( _zipFile, &posInfo ) == UNZ_OK )
            {
                std::string currentFileName = szCurrentFileName;

                ZipEntryInfo entry;
                entry.pos = posInfo;
                entry.uncompressed_size = ( uLong )fileInfo.uncompressed_size;
                _files[ currentFileName ] = entry;
            }
            err = unzGoToNextFile2( _zipFile, &fileInfo,
                                    szCurrentFileName, sizeof( szCurrentFileName ) - 1, nullptr, 0, nullptr, 0 );
        }

        return true;
    }

    char* KFExcelZip::GetFileData( const char* filename, unsigned long& size )
    {
        char* pBuffer = nullptr;

        auto it = _files.find( filename );

        if ( it == _files.end() )
        {
            return nullptr;
        }

        ZipEntryInfo fileInfo = it->second;

        int nRet = unzGoToFilePos( _zipFile, &fileInfo.pos );
        if ( UNZ_OK != nRet )
        {
            return nullptr;
        }

        nRet = unzOpenCurrentFile( _zipFile );
        if ( UNZ_OK != nRet )
        {
            return nullptr;
        }

        pBuffer = new char[ fileInfo.uncompressed_size ];
        unzReadCurrentFile( _zipFile, pBuffer, fileInfo.uncompressed_size );

        size = fileInfo.uncompressed_size;
        unzCloseCurrentFile( _zipFile );

        return pBuffer;
    }

    bool KFExcelZip::OpenXML( const char* filename, rapidxml::xml_document<>& doc )
    {
        unsigned long size = 0;
        char* data = GetFileData( filename, size );

        if ( data == nullptr )
        {
            return false;
        }

        char* pData = new char[ size + 1 ];
        strncpy( pData, data, size );
        pData[ size ] = 0;
        doc.parse<0>( pData );

        if ( data != nullptr )
        {
            delete[] data;
        }

        return true;
    }

}
