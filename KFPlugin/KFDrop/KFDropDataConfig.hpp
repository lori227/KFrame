#ifndef __KF_DROP_DATA_CONFIG_H__
#define __KF_DROP_DATA_CONFIG_H__

#include "KFConfig.h"
#include "KFDropData.h"

namespace KFrame
{
    class KFDropDataWeight : public KFWeight
    {
    public:
        // 掉落权重
        KFDropData _drop_data;
    };

    class KFDropDataSetting : public KFIntSetting
    {
    public:
        // 掉落数据
        KFWeightList<KFDropDataWeight> _drop_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////
    class KFDropDataConfig : public KFConfigT<KFDropDataSetting>, public KFInstance<KFDropDataConfig>
    {
    public:
        KFDropDataConfig()
        {
            _key_name = "id";
            _file_name = "dropdata";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _setting_list._objects )
            {
                auto setting = iter.second;
                for ( auto drop_weight : setting->_drop_data_list._weight_data )
                {
                    auto drop_data = &drop_weight->_drop_data;
                    if ( !drop_data->_data_name.empty() )
                    {
                        KFGlobal::Instance()->FormatElement( drop_data->_elements, drop_data->_data_name, drop_data->_data_value, drop_data->_data_key );
                    }
                }
            }
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFDropDataSetting> setting )
        {
            auto id = setting->_drop_data_list._weight_data.size();
            auto weight = xml_node.ReadUInt32( "weight", true );
            auto drop_data_weight = setting->_drop_data_list.Create( ++id, weight );

            drop_data_weight->_drop_data._data_index = id;
            drop_data_weight->_drop_data._drop_data_id = setting->_id;
            drop_data_weight->_drop_data._logic_name = xml_node.ReadString( "logicname", true );
            drop_data_weight->_drop_data._data_name = xml_node.ReadString( "dataname", true );
            drop_data_weight->_drop_data._data_key = xml_node.ReadUInt32( "dropkey", true );

            auto drop_value = xml_node.ReadString( "dropvalue", true );
            drop_data_weight->_drop_data._data_value = drop_value;
            drop_data_weight->_drop_data._data_range.SetValue( drop_value );
        }
    };
}

#endif