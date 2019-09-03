#include "KFDropConfig.hpp"
#include "KFZConfig/KFElementConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropConfig::ReadSetting( KFNode& xmlnode, KFDropSetting* kfsetting )
    {
        if ( kfsetting->_var_id == 0u )
        {
            kfsetting->_var_id = xmlnode.GetUInt32( "Var", true );
        }

        KFDropWeight* kfdropweight = nullptr;
        auto isdrop = xmlnode.GetBoolen( "IsDrop", true );
        if ( isdrop )
        {
            kfdropweight = __KF_NEW__( KFDropWeight );
            kfsetting->_necessary_list.Add( kfdropweight );
        }
        else
        {
            auto id = kfsetting->_rand_list._weight_data.size();
            auto weight = xmlnode.GetUInt32( "Weight", true );
            kfdropweight = kfsetting->_rand_list.Create( ++id, weight );
        }

        kfdropweight->_is_clear_var = xmlnode.GetBoolen( "Clear", true );
        kfdropweight->_drop_data._data_name = xmlnode.GetString( "DropName" );
        kfdropweight->_drop_data._data_key = xmlnode.GetUInt32( "DropKey", true );
        auto strvalue = xmlnode.GetString( "DropValue" );
        kfdropweight->_drop_data._data_value = strvalue;

        kfdropweight->_drop_data._min_value = KFUtility::SplitValue<uint32>( strvalue, FUNCTION_RANGE_STRING );
        kfdropweight->_drop_data._max_value = KFUtility::SplitValue<uint32>( strvalue, FUNCTION_RANGE_STRING );

        // 条件
        auto strcondition = xmlnode.GetString( "Condition", true );
        kfdropweight->_conditions.Parse( strcondition, kfsetting->_id, __FUNC_LINE__ );
    }

    void KFDropConfig::LoadAllComplete()
    {
        for ( auto iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto kfdropweight : kfsetting->_necessary_list._objects )
            {
                FormatDropElement( kfdropweight );
            }

            for ( auto kfdropweight : kfsetting->_rand_list._weight_data )
            {
                FormatDropElement( kfdropweight );
            }
        }
    }

    void KFDropConfig::FormatDropElement( KFDropWeight* dropweight )
    {
        // 掉落
        {
            auto dropdata = &dropweight->_drop_data;
            auto kfelementsetting = KFElementConfig::Instance()->FindElementSetting( dropdata->_data_name );
            auto strelement = __FORMAT__( kfelementsetting->_element_template, dropdata->_data_name, dropdata->_data_value, dropdata->_data_key );
            dropdata->_elements.Parse( strelement, __FUNC_LINE__ );
        }
    }
}