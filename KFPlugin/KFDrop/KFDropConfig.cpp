#include "KFDropConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropConfig::ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting )
    {
        kfsetting->_type = xmlnode.GetUInt32( "Type" );
        kfsetting->_var_id = xmlnode.GetUInt32( "Var", true );
        kfsetting->_max_count = xmlnode.GetUInt32( "MaxCount", true );

        auto probability = xmlnode.GetUInt32( "Probability" );
        auto dropdata = kfsetting->_drop_list.Create( 0, probability );
        dropdata->_is_drop = xmlnode.GetBoolen( "IsDrop", true );

        auto strdata = xmlnode.GetString( "DropData" );
        dropdata->_data.Parse( strdata, __FUNC_LINE__ );
    }
}