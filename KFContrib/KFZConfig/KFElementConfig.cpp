#include "KFElementConfig.hpp"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFElementConfig::ReadSetting( KFNode& xmlnode, KFElementSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_element_template = xmlnode.GetString( "Template" );
    }

    const KFElementSetting* KFElementConfig::FindElementSetting( const std::string& name )
    {
        static const std::string _default = "default";
        auto kfsetting = FindSetting( name );
        if ( kfsetting == nullptr )
        {
            kfsetting = FindSetting( _default );
        }

        return kfsetting;
    }
}