#include "KFDataConfig.hpp"

namespace KFrame
{
    const KFDataSetting* KFDataConfig::FindDataSetting( const std::string& classname, const std::string& dataname )
    {
        auto classsetting = FindSetting( classname );
        if ( classsetting == nullptr )
        {
            return nullptr;
        }

        return classsetting->FindSetting( dataname );
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static std::unordered_map< std::string, uint32 > _data_mask_list =
    {
        {"Show", KFDataDefine::Mask_Show},
        {"Multiple", KFDataDefine::Mask_Multiple},
        {"Save", KFDataDefine::Mask_Save},
        {"Sync", KFDataDefine::Mask_Client},
        {"Delay", KFDataDefine::Mask_Delay},
        {"View", KFDataDefine::Mask_View},
        {"Public", KFDataDefine::Mask_Basic},
        {"Relation", KFDataDefine::Mask_Relation},
        {"Team", KFDataDefine::Mask_Team},
        {"Guild", KFDataDefine::Mask_Guild},
        {"Rank", KFDataDefine::Mask_Rank},
        {"AddCall", KFDataDefine::Mask_AddCall},
        {"RemoveCall", KFDataDefine::Mask_RemoveCall},
        {"UpdateCall", KFDataDefine::Mask_UpdataCall},
    };

    void KFDataConfig::ReadSetting( KFNode& xmlnode, KFClassSetting* kfsetting )
    {
        auto dataname = xmlnode.GetString( "Name" );
        auto kfdatasetting = kfsetting->_static_data.Create( dataname );
        kfdatasetting->_name = dataname;

        kfdatasetting->_logic_name = xmlnode.GetString( "LogicName", true );
        if ( kfdatasetting->_logic_name.empty() )
        {
            kfdatasetting->_logic_name = kfdatasetting->_name;
        }
        else
        {
            kfsetting->_logic_name_list[ kfdatasetting->_logic_name ] = kfdatasetting->_name;
        }

        kfdatasetting->_type = KFDataDefine::ConvertDataType( xmlnode.GetString( "Type" ) );
        kfdatasetting->_logic_type = kfdatasetting->_type;
        kfdatasetting->_contain_class = xmlnode.GetString( "ContainClass", true );
        if ( !kfdatasetting->_contain_class.empty() )
        {
            auto type = KFDataDefine::ConvertDataType( kfdatasetting->_contain_class );
            if ( type != KFDataDefine::Type_Unknow )
            {
                kfdatasetting->_logic_type = type;
            }
            else
            {
                kfdatasetting->_logic_type = KFDataDefine::Type_Object;
            }
        }

        switch ( kfdatasetting->_type )
        {
        case KFDataDefine::Type_Int32:
            kfdatasetting->_int_max_value = __MAX_INT32__;
            break;
        case KFDataDefine::Type_UInt32:
            kfdatasetting->_int_max_value = __MAX_UINT32__;
            break;
        }

        kfdatasetting->_delay_save_time = xmlnode.GetUInt32( "SaveTime", true );
        kfdatasetting->_delete_type = xmlnode.GetUInt32( "DeleteType", true );
        kfdatasetting->_str_init_value = xmlnode.GetString( "InitValue", true );
        kfdatasetting->_str_min_value = xmlnode.GetString( "MinValue", true );
        kfdatasetting->_str_max_value = xmlnode.GetString( "MaxValue", true );
        kfdatasetting->_str_logic_value = xmlnode.GetString( "LogicValue", true );
        kfdatasetting->_key_name = xmlnode.GetString( "KeyName", true );
        kfdatasetting->_config_key_name = xmlnode.GetString( "ConfigKeyName", true );
        kfdatasetting->_value_key_name = xmlnode.GetString( "ValueKeyName", true );
        kfdatasetting->_lua_file = xmlnode.GetString( "LuaFile", true );
        kfdatasetting->_add_function = xmlnode.GetString( "AddFunction", true );
        kfdatasetting->_update_function = xmlnode.GetString( "UpdateFunction", true );
        kfdatasetting->_remove_function = xmlnode.GetString( "RemoveFunction", true );

        for ( auto& iter : _data_mask_list )
        {
            if ( xmlnode.GetString( iter.first.c_str(), true ) == "1" )
            {
                KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, iter.second );
            }
        }
    }

    void KFDataConfig::LoadComplete()
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


    void KFDataConfig::LoadAllComplete()
    {
        auto kfglobal = KFGlobal::Instance();
        for ( auto& iter : KFDataConfig::Instance()->_settings._objects )
        {
            auto kfclasssetting = iter.second;

            for ( auto& siter : kfclasssetting->_static_data._objects )
            {
                auto kfdatasetting = siter.second;

                // 初始值
                if ( kfdatasetting->_str_init_value != _invalid_string )
                {
                    if ( KFUtility::IsNumber( kfdatasetting->_str_init_value ) )
                    {
                        kfdatasetting->_int_init_value = __TO_UINT32__( kfdatasetting->_str_init_value );
                    }
                    else
                    {
                        kfdatasetting->_int_init_value = kfglobal->GetUInt32( kfdatasetting->_str_init_value );
                    }
                }

                // 最小值
                if ( kfdatasetting->_str_min_value != _invalid_string )
                {
                    if ( KFUtility::IsNumber( kfdatasetting->_str_min_value ) )
                    {
                        kfdatasetting->_int_min_value = __TO_UINT32__( kfdatasetting->_str_min_value );
                    }
                    else
                    {
                        kfdatasetting->_int_min_value = kfglobal->GetUInt32( kfdatasetting->_str_min_value );
                    }
                }

                // 最大值
                if ( kfdatasetting->_str_max_value != _invalid_string )
                {
                    if ( KFUtility::IsNumber( kfdatasetting->_str_max_value ) )
                    {
                        kfdatasetting->_int_max_value = __TO_UINT32__( kfdatasetting->_str_max_value );
                    }
                    else
                    {
                        kfdatasetting->_int_max_value = kfglobal->GetUInt32( kfdatasetting->_str_max_value );
                    }
                }

                // 逻辑值
                if ( kfdatasetting->_str_logic_value != _invalid_string )
                {
                    if ( KFUtility::IsNumber( kfdatasetting->_str_logic_value ) )
                    {
                        kfdatasetting->_int_logic_value = __TO_UINT32__( kfdatasetting->_str_logic_value );
                    }
                    else
                    {
                        kfdatasetting->_int_logic_value = kfglobal->GetUInt32( kfdatasetting->_str_logic_value );
                    }
                }
            }
        }
    }
}