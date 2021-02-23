#include "KFDropDataConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropDataConfig::ReadSetting( KFXmlNode& xml_node, KFDropDataSetting* setting )
    {
        auto id = setting->_drop_data_list._weight_data.size();
        auto weight = xml_node.ReadUInt32( "weight", true );
        auto kfdropdataweight = setting->_drop_data_list.Create( ++id, weight );

        kfdropdataweight->_drop_data._data_index = id;
        kfdropdataweight->_drop_data._drop_data_id = setting->_id;
        kfdropdataweight->_drop_data._logic_name = xml_node.ReadString( "logicname", true );
        kfdropdataweight->_drop_data._data_name = xml_node.ReadString( "data_name", true );
        kfdropdataweight->_drop_data._data_key = xml_node.ReadUInt32( "dropkey", true );

        auto strdropvalue = xml_node.ReadString( "dropvalue", true );
        kfdropdataweight->_drop_data._data_value = strdropvalue;
        kfdropdataweight->_drop_data._data_range.SetValue( strdropvalue );
    }

    void KFDropDataConfig::LoadAllComplete()
    {
        for ( auto& iter : _setting_list._objects )
        {
            auto setting = iter.second;
            for ( auto kfdropweight : setting->_drop_data_list._weight_data )
            {
                auto dropdata = &kfdropweight->_drop_data;
                if ( !dropdata->_data_name.empty() )
                {
                    KFGlobal::Instance()->FormatElement( dropdata->_elements, dropdata->_data_name, dropdata->_data_value, dropdata->_data_key );
                }
            }
        }
    }
}