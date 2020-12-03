#include "KFTar.h"
#include "tar.hpp"
#include "Poco/File.h"

namespace KFrame
{
    void RecursionTarPath( Tar& tar, const std::string& path )
    {
        Poco::File file( path );
        std::vector< Poco::File > filelist;
        file.list( filelist );

        for ( auto& pocofile : filelist )
        {
            if ( pocofile.isHidden() )
            {
                continue;
            }

            if ( pocofile.isFile() )
            {
                tar.putFile( pocofile.path().c_str(), pocofile.path().c_str() );
            }
            else if ( pocofile.isDirectory() )
            {
                RecursionTarPath( tar, pocofile.path() );
            }
        }
    }

    bool KFTar::TarPath( const std::string& path, const std::string& tarname )
    {
        Tar tar( tarname.c_str() );

        Poco::File file( path );
        std::vector< Poco::File > filelist;
        file.list( filelist );

        for ( auto& pocofile : filelist )
        {
            if ( pocofile.isHidden() )
            {
                continue;
            }

            if ( pocofile.isFile() )
            {
                tar.putFile( pocofile.path().c_str(), pocofile.path().c_str() );
            }
            else if ( pocofile.isDirectory() )
            {
                RecursionTarPath( tar, pocofile.path() );
            }
        }
        tar.close();
        return true;
    }


}
