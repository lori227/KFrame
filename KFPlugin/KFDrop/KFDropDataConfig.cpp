#include "KFDropDataConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropDataConfig::ReadSetting( KFNode& xmlnode, KFDropDataSetting* kfsetting )
    {
        auto id = kfsetting->_drop_data_list._weight_data.size();
        auto weight = xmlnode.GetUInt32( "Weight", true );
        auto kfdropdataweight = kfsetting->_drop_data_list.Create( ++id, weight );

        kfdropdataweight->_drop_data._data_index = id;
        kfdropdataweight->_drop_data._drop_data_id = kfsetting->_id;
        kfdropdataweight->_drop_data._logic_name = xmlnode.GetString( "LogicName" );
        kfdropdataweight->_drop_data._data_name = xmlnode.GetString( "DataName" );
        kfdropdataweight->_drop_data._data_key = xmlnode.GetUInt32( "DropKey", true );

        auto strdropvalue = xmlnode.GetString( "DropValue" );
        kfdropdataweight->_drop_data._data_value = strdropvalue;
        kfdropdataweight->_drop_data._min_value = KFUtility::SplitValue<uint32>( strdropvalue, __RANGE_STRING__ );
        kfdropdataweight->_drop_data._max_value = KFUtility::SplitValue<uint32>( strdropvalue, __RANGE_STRING__ );
        if ( kfdropdataweight->_drop_data._max_value == 0u )
        {
            kfdropdataweight->_drop_data._max_value = kfdropdataweight->_drop_data._min_value;
        }
    }

    void KFDropDataConfig::LoadAllComplete()
    {
        for ( auto& iter : _settings._objects )
        {
            auto kfsetting = iter.second;
            for ( auto kfdropweight : kfsetting->_drop_data_list._weight_data )
            {
                auto dropdata = &kfdropweight->_drop_data;
                if ( !dropdata->_data_name.empty() )
                {
                    KFElementConfig::Instance()->FormatElement( dropdata->_elements, dropdata->_data_name, dropdata->_data_value, dropdata->_data_key );
                }
            }
        }
    }
}