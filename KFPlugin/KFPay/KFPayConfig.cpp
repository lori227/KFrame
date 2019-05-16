#include "KFPayConfig.hpp"

namespace KFrame
{
    bool KFPayConfig::LoadConfig()
    {
        _pay_list.Clear();
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetString( "Id" );
            auto kfsetting = _pay_list.Create( id );

            kfsetting->_id = id;
            kfsetting->_price = xmlnode.GetUInt32( "Price" );

            auto strbuyelement = xmlnode.GetString( "BuyElement" );
            kfsetting->_buy_elements.Parse( strbuyelement, __FUNC_LINE__ );

            kfsetting->_first_variable_id = xmlnode.GetUInt32( "FirstVarId" );

            auto strfirstelement = xmlnode.GetString( "FirstElement" );
            kfsetting->_first_elements.Parse( strfirstelement, __FUNC_LINE__ );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    const KFPaySetting* KFPayConfig::FindPaySetting( const std::string& id ) const
    {
        return _pay_list.Find( id );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
}