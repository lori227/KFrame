#include "KFHeroConfig.hpp"

namespace KFrame
{
    void KFHeroConfig::ReadSetting( KFNode& xml_node, KFHeroSetting* kfsetting )
    {
        kfsetting->_skin_list.clear();
        auto strskins = xml_node.GetString( "Skins" );
        while ( !strskins.empty() )
        {
            auto skinid = KFUtility::SplitValue<uint32>( strskins, __SPLIT_STRING__ );
            if ( skinid != _invalid_int )
            {
                kfsetting->_skin_list.insert( skinid );
            }
        }

    }
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFHeroSetting::HaveSkin( uint32 skinid ) const
    {
        return _skin_list.find( skinid ) != _skin_list.end();
    }
}