#include "KFPayConfig.hpp"

namespace KFrame
{
    void KFPayConfig::ReadSetting( KFNode& xmlnode, KFPaySetting* kfsetting )
    {
        kfsetting->_price = xmlnode.GetUInt32( "Price" );

        auto strbuyelement = xmlnode.GetString( "BuyElement" );
        kfsetting->_buy_elements.Parse( strbuyelement, __FUNC_LINE__ );

        kfsetting->_first_variable_id = xmlnode.GetUInt32( "FirstVarId" );

        auto strfirstelement = xmlnode.GetString( "FirstElement" );
        kfsetting->_first_elements.Parse( strfirstelement, __FUNC_LINE__ );
    }
}