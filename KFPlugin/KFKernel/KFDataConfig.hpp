#ifndef __KF_DATA_CONFIG_H__
#define __KF_DATA_CONFIG_H__

#include "KFConfig.h"
#include "KFDataSetting.h"
#include "KFConfig/KFConstantConfig.hpp"

namespace KFrame
{
    static StringUInt32 _data_mask_list =
    {
        {"show", KFDataDefine::DataMaskShow},
        {"multiple", KFDataDefine::DataMaskMultiple},
        {"save", KFDataDefine::DataMaskSave},
        {"sync", KFDataDefine::DataMaskSync},
        {"request", KFDataDefine::DataMaskRequest},
        {"view", KFDataDefine::DataMaskView},
        {"basic", KFDataDefine::DataMaskBasic},
        {"relation", KFDataDefine::DataMaskRelation},
        {"team", KFDataDefine::DataMaskTeam},
        {"guild", KFDataDefine::DataMaskGuild},
        {"rank", KFDataDefine::DataMaskRank},
        {"shield", KFDataDefine::DataMaskShield},
        {"client", KFDataDefine::DataMaskClient},
        {"addcall", KFDataDefine::DataMaskAddCall},
        {"removecall", KFDataDefine::DataMaskRemoveCall},
        {"updatecall", KFDataDefine::DataMaskUpdateCall},
    };


#define __INIT_INT_VALUE__( str_value, int_value )\
    if ( str_value != _invalid_string )\
    {\
        if ( KFUtility::IsNumber( str_value ) )\
        {\
            int_value = __TO_UINT32__( str_value );\
        }\
        else\
        {                                       \
            auto setting = KFConstantConfig::Instance()->FindSetting( str_value ); \
            if ( setting != nullptr )         \
            {                                   \
                int_value = __TO_UINT32__( setting->_value );\
            }\
        }\
    }

    class KFDataConfig : public KFConfigT< KFClassSetting >, public KFInstance< KFDataConfig >
    {
    public:
        KFDataConfig()
        {
            _key_name = "id";
            _file_name = "class";
        }

        // 加载完成
        virtual void LoadComplete()
        {
            // 初始化包含的类, 优化每次创建都要去查找的问题
            for ( auto& iter : _settings._objects )
            {
                auto class_setting = iter.second;
                for ( auto& data_iter : class_setting->_static_data._objects )
                {
                    auto data_setting = data_iter.second;
                    if ( !data_setting->_contain_class.empty() )
                    {
                        auto find_setting = FindSetting( data_setting->_contain_class );
                        if ( find_setting != nullptr )
                        {
                            data_setting->_class_setting = find_setting.get();
                        }
                    }
                }
            }
        }

        // 所有加载完成
        virtual void LoadAllComplete()
        {
            for ( auto& iter : KFDataConfig::Instance()->_settings._objects )
            {
                auto class_setting = iter.second;

                for ( auto& data_iter : class_setting->_static_data._objects )
                {
                    auto data_setting = data_iter.second;

                    // 初始值
                    __INIT_INT_VALUE__( data_setting->_str_init_value, data_setting->_int_init_value );

                    // 最小值
                    __INIT_INT_VALUE__( data_setting->_str_min_value, data_setting->_int_min_value );

                    // 最大值
                    __INIT_INT_VALUE__( data_setting->_str_max_value, data_setting->_int_max_value );

                    // 运行时参数
                    __INIT_INT_VALUE__( data_setting->_str_run_param, data_setting->_int_run_param );

                    // 数组, 集合的最大容量
                    __INIT_INT_VALUE__( data_setting->_str_max_capacity, data_setting->_int_max_capacity );
                }
            }
        }

        // 获得属性配置
        std::shared_ptr<const KFDataSetting> FindDataSetting( const std::string& class_name, const std::string& data_name )
        {
            auto class_setting = FindSetting( class_name );
            if ( class_setting == nullptr )
            {
                return nullptr;
            }

            return class_setting->FindSetting( data_name );
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xml_node, std::shared_ptr<KFClassSetting> class_setting )
        {
            auto data_name = xml_node.ReadString( "name", true );
            auto data_setting = class_setting->_static_data.Create( data_name );
            data_setting->_name = data_name;

            data_setting->_logic_name = xml_node.ReadString( "logicname", true );
            if ( data_setting->_logic_name.empty() )
            {
                data_setting->_logic_name = data_setting->_name;
            }
            else
            {
                class_setting->_logic_name_list[ data_setting->_logic_name ] = data_setting->_name;
            }

            data_setting->_type = KFDataDefine::ConvertDataType( xml_node.ReadString( "type", true ) );
            data_setting->_logic_type = data_setting->_type;
            switch ( data_setting->_type )
            {
            case KFDataDefine::DataTypeInt32:
                data_setting->_int_max_value = __MAX_INT32__;
                break;
            case KFDataDefine::DataTypeUInt32:
                data_setting->_int_max_value = __MAX_UINT32__;
                break;
            case KFDataDefine::DataTypeInt64:
                data_setting->_int_max_value = __MAX_INT64__;
                break;
            case KFDataDefine::DataTypeUInt64:
                data_setting->_int_max_value = __MAX_UINT64__;
                break;
            }

            data_setting->_contain_class = xml_node.ReadString( "containclass", true );
            if ( !data_setting->_contain_class.empty() )
            {
                auto type = KFDataDefine::ConvertDataType( data_setting->_contain_class );
                if ( type != KFDataDefine::DataTypeUnknown )
                {
                    data_setting->_logic_type = type;
                }
                else
                {
                    data_setting->_logic_type = KFDataDefine::DataTypeObject;
                }
            }

            data_setting->_delay_save_time = xml_node.ReadUInt32( "savetime", true );
            data_setting->_delete_type = xml_node.ReadUInt32( "deletetype", true );
            data_setting->_str_init_value = xml_node.ReadString( "initvalue", true );
            data_setting->_str_min_value = xml_node.ReadString( "minvalue", true );
            data_setting->_str_max_value = xml_node.ReadString( "maxvalue", true );
            data_setting->_str_run_param = xml_node.ReadString( "runparam", true );
            data_setting->_str_max_capacity = xml_node.ReadString( "capacity", true );
            data_setting->_key_name = xml_node.ReadString( "keyname", true );
            data_setting->_config_key_name = xml_node.ReadString( "configkeyname", true );
            data_setting->_value_key_name = xml_node.ReadString( "valuekeyname", true );
            data_setting->_lua_file = xml_node.ReadString( "luafile", true );
            data_setting->_add_function = xml_node.ReadString( "addfunction", true );
            data_setting->_update_function = xml_node.ReadString( "updatefunction", true );
            data_setting->_remove_function = xml_node.ReadString( "removefunction", true );

            data_setting->_delay_online_sync_time = xml_node.ReadUInt32( "delay", true );
            if ( data_setting->_delay_online_sync_time > 0u )
            {
                class_setting->_online_sync_time.insert( data_setting->_delay_online_sync_time );
            }

            for ( auto& iter : _data_mask_list )
            {
                if ( xml_node.ReadString( iter.first.c_str(), true ) == "1" )
                {
                    KFUtility::AddBitMask< uint32 >( data_setting->_data_mask, iter.second );
                }
            }
        }
    };
}

#endif