#include "KFGenerateVersion.h"


namespace KFrame
{
    void KFGenerateVersion::AddVersion( const std::string& path, const std::string& filename, const std::string& version )
    {
        KFLocker locker( _kf_mutex );
        auto kfversion = _versions.Create( path );
        kfversion->AddVersion( filename, version );
    }

    void KFGenerateVersion::LoadVersionXml( const std::string& path )
    {
        if ( path.empty() )
        {
            return;
        }

        auto kfversion = _versions.Create( path );
        kfversion->_version_list.clear();

        try
        {
            auto filename = __FORMAT__( "{}/version.xml", path );
            KFXml kfxml( filename );

            auto root = kfxml.RootNode();
            if ( !root.IsValid() )
            {
                return;
            }

            auto node = root.FindNode( __STRING__( node ).c_str() );
            while ( node.IsValid() )
            {
                auto file = node.ReadString( "name" );
                auto verson = node.ReadString( "version" );
                kfversion->AddVersion( file, verson );
                node.NextNode();
            }
        }
        catch ( ... )
        {
        }
    }

    void KFGenerateVersion::SaveVersionXml( const std::string& path )
    {
        if ( path.empty() )
        {
            return;
        }

        auto kfversion = _versions.Find( path );
        if ( kfversion == nullptr )
        {
            return;
        }


        auto filename = __FORMAT__( "{}/version.xml", path );
        std::ofstream xmlfile( filename );
        if ( !xmlfile )
        {
            return;
        }

        xmlfile << __FORMAT__( "<{}>\n", __STRING__( root ) );
        for ( auto& iter : kfversion->_version_list )
        {
            auto rowdata = &iter;
            xmlfile << __FORMAT__( "\t<{} name=\"{}\" version=\"{}\"/>\n", __STRING__( node ), iter.first, iter.second );
        }
        xmlfile << __FORMAT__( "</{}>\n", __STRING__( root ) );

        xmlfile.flush();
        xmlfile.close();
    }
}
