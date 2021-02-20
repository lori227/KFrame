#include "KFDropDataConfig.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    void KFDropDataConfig::ReadSetting( KFXmlNode& xmlnode, KFDropDataSetting* kfsetting )
    {
        auto id = kfsetting->_drop_data_list._weight_data.size();
        auto weight = xmlnode.ReadUInt32( "weight", true );
        auto kfdropdataweight = kfsetting->_drop_data_list.Create( ++id, weight );

        kfdropdataweight->_drop_data._data_index = id;
        kfdropdataweight->_drop_data._drop_data_id = kfsetting->_id;
        kfdropdataweight->_drop_data._logic_name = xmlnode.ReadString( "logicname", true );
        kfdropdataweight->_drop_data._data_name = xmlnode.ReadString( "data_name", true );
        kfdropdataweight->_drop_data._data_key = xmlnode.ReadUInt32( "dropkey", true );

        auto strdropvalue = xmlnode.ReadString( "dropvalue", true );
        kfdropdataweight->_drop_data._data_value = strdropvalue;
        kfdropdataweight->_drop_data._data_range.SetValue( strdropvalue );
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
                    KFGlobal::Instance()->FormatElement( dropdata->_elements, dropdata->_data_name, dropdata->_data_value, dropdata->_data_key );
                }
            }
        }
    }
}