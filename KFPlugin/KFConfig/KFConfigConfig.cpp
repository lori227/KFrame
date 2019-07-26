#include "KFConfigConfig.hpp"


namespace KFrame
{
    static std::map< std::string, uint32 > _mask_list =
    {
        {"Reload", KFConfigEnum::CanReload},
        {"ClearData", KFConfigEnum::CanReload},
    };

    void KFConfigConfig::ReadSetting( KFNode& xmlnode, KFConfigSetting* kfsetting )
    {
        auto filename = xmlnode.GetString( "FileName" );
        auto configfile = KFUtility::FormatConfigFile( filename, KFGlobal::Instance()->_channel, KFGlobal::Instance()->_service );
        kfsetting->_file_list.push_back( configfile );

        for ( auto& iter : _mask_list )
        {
            if ( xmlnode.GetString( iter.first.c_str(), true ) == "1" )
            {
                KFUtility::AddBitMask< uint32 >( kfsetting->_load_mask, iter.second );
            }
        }
    }

    const std::string& KFConfigSetting::IsFile( const std::string& file ) const
    {
        for ( auto& filename : _file_list )
        {
            auto pos = filename.find( file );
            if ( pos != std::string::npos )
            {
                return filename;
            }
        }

        return _invalid_str;
    }

}