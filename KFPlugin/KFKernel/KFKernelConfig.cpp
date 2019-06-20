#include "KFKernelConfig.hpp"

namespace KFrame
{
    const KFDataSetting* KFKernelConfig::FindDataSetting( const std::string& classname, const std::string& dataname )
    {
        auto classsetting = FindSetting( classname );
        if ( classsetting == nullptr )
        {
            return nullptr;
        }

        return classsetting->FindSetting( dataname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFKernelConfig::ReadSetting( KFNode& xmlnode, KFClassSetting* kfsetting )
    {
        auto dataname = xmlnode.GetString( "Name" );
        auto kfdatasetting = kfsetting->_static_data.Create( dataname );
        kfdatasetting->_name = dataname;
        kfdatasetting->_type = KFDataDefine::ConvertDataType( xmlnode.GetString( "Type" ) );
        kfdatasetting->_contain_class = xmlnode.GetString( "ContainClass" );
        kfdatasetting->_init_value = xmlnode.GetString( "InitValue" );
        kfdatasetting->_max_value = xmlnode.GetUInt32( "MaxValue" );
        kfdatasetting->_key_name = xmlnode.GetString( "KeyName" );
        kfdatasetting->_config_key_name = xmlnode.GetString( "ConfigKeyName" );
        kfdatasetting->_lua_file = xmlnode.GetString( "LuaFile" );
        kfdatasetting->_add_function = xmlnode.GetString( "AddFunction" );
        kfdatasetting->_update_function = xmlnode.GetString( "UpdateFunction" );
        kfdatasetting->_remove_function = xmlnode.GetString( "RemoveFunction" );

        if ( xmlnode.GetString( "Create" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Create );
        }
        if ( xmlnode.GetString( "Show" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Show );
        }
        if ( xmlnode.GetString( "Multiple" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Multiple );
        }
        if ( xmlnode.GetString( "Save" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Save );
        }
        if ( xmlnode.GetString( "Sync" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Client );
        }
        if ( xmlnode.GetString( "View" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_View );
        }
        if ( xmlnode.GetString( "Log" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Log );
        }
        if ( xmlnode.GetString( "Public" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Public );
        }
        if ( xmlnode.GetString( "Relation" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Relation );
        }
        if ( xmlnode.GetString( "Group" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Group );
        }
        if ( xmlnode.GetString( "Guild" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Guild );
        }
        if ( xmlnode.GetString( "Rank" ) == "1" )
        {
            KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, KFDataDefine::Mask_Rank );
        }
    }

    void KFKernelConfig::LoadComplete()
    {
        // 初始化包含的类, 优化每次创建都要去查找的问题
        for ( auto& iter : _settings._objects )
        {
            auto kfclasssetting = iter.second;
            for ( auto& dataiter : kfclasssetting->_static_data._objects )
            {
                auto kfdatasetting = dataiter.second;
                if ( !kfdatasetting->_contain_class.empty() )
                {
                    kfdatasetting->_class_setting = FindSetting( kfdatasetting->_contain_class );
                }
            }
        }
    }
}