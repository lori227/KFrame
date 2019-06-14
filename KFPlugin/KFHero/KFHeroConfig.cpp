#include "KFHeroConfig.hpp"

namespace KFrame
{
    bool KFHeroSetting::HaveSkin( uint32 skinid ) const
    {
        return _skin_list.find( skinid ) != _skin_list.end();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    const KFHeroSetting* KFHeroConfig::FindHeroSetting( uint32 heroid ) const
    {
        return _hero_setting.Find( heroid );
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFHeroConfig::LoadConfig( const std::string& file )
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "item" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _hero_setting.Create( id );

            kfsetting->_id = id;

            kfsetting->_skin_list.clear();
            auto strskins = xmlnode.GetString( "Skins" );
            while ( !strskins.empty() )
            {
                auto skinid = KFUtility::SplitValue< uint32 >( strskins, DEFAULT_SPLIT_STRING );
                if ( skinid != _invalid_int )
                {
                    kfsetting->_skin_list.insert( skinid );
                }
            }

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}