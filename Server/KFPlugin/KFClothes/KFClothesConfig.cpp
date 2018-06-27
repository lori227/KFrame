#include "KFClothesConfig.h"

namespace KFrame
{
	///////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////
	KFClothesConfig::KFClothesConfig()
	{

	}

	KFClothesConfig::~KFClothesConfig()
	{

	}

	const KFClothesSetting* KFClothesConfig::FindClothesSetting( uint32 id ) const
	{
		return _clothes_setting.Find( id );
	}

	void KFClothesConfig::AddClothesSetting( KFClothesSetting* kfsetting )
	{
		_clothes_setting.Insert( kfsetting->_id, kfsetting );
	}

	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	bool KFClothesConfig::LoadConfig( const char* file )
	{
		_clothes_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			//////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Setting" );
			while ( xmlnode.IsValid() )
			{
				auto kfsetting = __KF_CREATE__( KFClothesSetting );

				kfsetting->_id = xmlnode.GetUInt32( "Id" );
				kfsetting->_quality = xmlnode.GetUInt32( "Quality" );
				kfsetting->_mail_id = xmlnode.GetUInt32( "MailId" );

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
				kfsetting->_exchange.ParseFromString( strexchange, __FUNCTION_LINE__ );

				AddClothesSetting( kfsetting );
				xmlnode.NextNode();
			}
		}
		catch ( ... )
		{
			return false;
		}

		return true;
	}
}