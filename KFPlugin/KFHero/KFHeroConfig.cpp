#include "KFHeroConfig.hpp"

namespace KFrame
{
    void KFHeroConfig::ReadSetting( KFNode& xmlnode, KFHeroSetting* kfsetting )
    {
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

    }
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFHeroSetting::HaveSkin( uint32 skinid ) const
    {
        return _skin_list.find( skinid ) != _skin_list.end();
    }
}