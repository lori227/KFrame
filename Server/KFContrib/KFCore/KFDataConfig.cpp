#include "KFDataConfig.h"
#include "KFXml/KFXml.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
	KFDataConfig::KFDataConfig()
	{

	}

	KFDataConfig::~KFDataConfig()
	{

	}

	const KFClassSetting* KFDataConfig::GetClassSetting( const std::string& classname )
	{
		return _class_setting.Find( classname );
	}

	void KFDataConfig::AddDataSetting( const std::string& classname, KFDataSetting& kfdatasetting )
	{
		auto kfclasssetting = _class_setting.Create( classname );
		kfclasssetting->_class = classname;
		kfclasssetting->AddDataSetting( kfdatasetting );
	}

	const KFDataSetting* KFDataConfig::GetDataSetting( const std::string& classname, const std::string& propertyname )
	{
		auto classsetting = GetClassSetting( classname );
		if ( classsetting == nullptr )
		{
			return nullptr;
		}

		return classsetting->GetDataSetting( propertyname );
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool KFDataConfig::LoadConfig( const char* file )
	{
		_class_setting.Clear();

		try
		{
			KFXml kfxml( file );
			auto config = kfxml.RootNode();
			
			//////////////////////////////////////////////////////////////////
			auto xmlnode = config.FindNode( "Setting" );
			while ( xmlnode.IsValid() )
			{
				auto classname = xmlnode.GetString( "Class" );

				KFDataSetting datasetting;
				datasetting._name = xmlnode.GetString( "Name" );
				datasetting._type = KFDataDefine::ConvertDataType( xmlnode.GetString( "Type" ) );
				datasetting._contain_class = xmlnode.GetString( "ContainClass" );
				datasetting._init_value = xmlnode.GetString( "InitValue" );
				datasetting._key_name = xmlnode.GetString( "KeyName" );
				datasetting._config_key_name = xmlnode.GetString( "ConfigKeyName" );
				datasetting._lua_file = xmlnode.GetString( "LuaFile" );
				datasetting._add_function = xmlnode.GetString( "AddFunction" );
				datasetting._update_function = xmlnode.GetString( "UpdateFunction" );
				datasetting._remove_function = xmlnode.GetString( "RemoveFunction" );

				///////////////////////////////////////////////////////////////////////////////////////////////
				if ( xmlnode.GetString( "AutoCreate" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Auto_Create );
				}

				if ( xmlnode.GetString( "Save" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Save_Database );
				}

				if ( xmlnode.GetString( "Sync" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Sync_Client );
				}

				if ( xmlnode.GetString( "View" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Sync_View );
				}

				if ( xmlnode.GetString( "Log" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_System_Log );
				}

				if (xmlnode.GetString( "Public" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Public_Data );
				}

				if ( xmlnode.GetString( "Online" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Online_Data );
				}

				if ( xmlnode.GetString( "Relation" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Relation_Data );
				}

				if ( xmlnode.GetString( "Group" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Group_Data );
				}

				if ( xmlnode.GetString( "Guild" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Guild_Data );
				}

				if ( xmlnode.GetString( "Rank" ) == "1" )
				{
					KFUtility::AddBitMask< uint32 >( datasetting._data_mask, KFDataDefine::Mask_Rank_Data );
				}
				///////////////////////////////////////////////////////////////////////////////////////////////
				AddDataSetting( classname, datasetting );
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