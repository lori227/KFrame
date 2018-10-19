#include "KFClothesConfig.h"

namespace KFrame
{
    const KFClothesSetting* KFClothesConfig::FindClothesSetting( uint32 id ) const
    {
        return _clothes_setting.Find( id );
    }

    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    bool KFClothesConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto xmlnode = config.FindNode( "Setting" );
        while ( xmlnode.IsValid() )
        {
            auto id = xmlnode.GetUInt32( "Id" );
            auto kfsetting = _clothes_setting.Create( id );

            kfsetting->_id = id;
            kfsetting->_quality = xmlnode.GetUInt32( "Quality" );
            kfsetting->_mail_id = xmlnode.GetUInt32( "MailId" );

            kfsetting->_model_list.clear();
            auto strmodel = xmlnode.GetString( "Model" );
            while ( !strmodel.empty() )
            {
                KFUtility::SplitString( strmodel, "[" );
                auto context = KFUtility::SplitString( strmodel, "]" );
                while ( !context.empty() )
                {
                    auto modelid = KFUtility::SplitValue< uint32 >( context, DEFAULT_SPLIT_STRING );
                    if ( modelid != _invalid_int )
                    {
                        kfsetting->_model_list.insert( modelid );
                    }
                }
            }

            auto strexchange = xmlnode.GetString( "Exchange" );
            kfsetting->_exchange.ParseFromString( strexchange, __FUNC_LINE__ );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}