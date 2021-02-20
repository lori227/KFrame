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
        {"addcall", KFDataDefine::DataMaskAddCall},
        {"removecall", KFDataDefine::DataMaskRemoveCall},
        {"updatecall", KFDataDefine::DataMaskUpdateCall},
        {"shield", KFDataDefine::DataMaskShield},
        {"client", KFDataDefine::DataMaskClient},
    };


#define __INIT_INT_VALUE__( strvalue, intvalue )\
    if ( strvalue != _invalid_string )\
    {\
        if ( KFUtility::IsNumber( strvalue ) )\
        {\
            intvalue = __TO_UINT32__( strvalue );\
        }\
        else\
        {                                       \
            auto kfsetting = KFConstantConfig::Instance()->FindSetting( strvalue ); \
            if ( kfsetting != nullptr )         \
            {                                   \
                intvalue= __TO_UINT32__( kfsetting->_value );\
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

        // 所有加载完成
        virtual void LoadAllComplete()
        {
            auto kfglobal = KFGlobal::Instance();
            for ( auto& iter : KFDataConfig::Instance()->_settings._objects )
            {
                auto kfclasssetting = iter.second;

                for ( auto& siter : kfclasssetting->_static_data._objects )
                {
                    auto kfdatasetting = siter.second;

                    // 初始值
                    __INIT_INT_VALUE__( kfdatasetting->_str_init_value, kfdatasetting->_int_init_value );

                    // 最小值
                    __INIT_INT_VALUE__( kfdatasetting->_str_min_value, kfdatasetting->_int_min_value );

                    // 最大值
                    __INIT_INT_VALUE__( kfdatasetting->_str_max_value, kfdatasetting->_int_max_value );

                    // 运行时参数
                    __INIT_INT_VALUE__( kfdatasetting->_str_run_param, kfdatasetting->_int_run_param );

                    // 数组, 集合的最大容量
                    __INIT_INT_VALUE__( kfdatasetting->_str_max_capacity, kfdatasetting->_int_max_capacity );
                }
            }
        }

        // 获得属性配置
        const KFDataSetting* FindDataSetting( const std::string& classname, const std::string& dataname )
        {
            auto classsetting = FindSetting( classname );
            if ( classsetting == nullptr )
            {
                return nullptr;
            }

            return classsetting->FindSetting( dataname );
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFClassSetting* kfsetting )
        {
            auto dataname = xmlnode.ReadString( "name", true );
            auto kfdatasetting = kfsetting->_static_data.Create( dataname );
            kfdatasetting->_name = dataname;

            kfdatasetting->_logic_name = xmlnode.ReadString( "logicname", true );
            if ( kfdatasetting->_logic_name.empty() )
            {
                kfdatasetting->_logic_name = kfdatasetting->_name;
            }
            else
            {
                kfsetting->_logic_name_list[ kfdatasetting->_logic_name ] = kfdatasetting->_name;
            }

            kfdatasetting->_type = KFDataDefine::ConvertDataType( xmlnode.ReadString( "type", true ) );
            kfdatasetting->_logic_type = kfdatasetting->_type;
            switch ( kfdatasetting->_type )
            {
            case KFDataDefine::DataTypeInt32:
                kfdatasetting->_int_max_value = __MAX_INT32__;
                break;
            case KFDataDefine::DataTypeUInt32:
                kfdatasetting->_int_max_value = __MAX_UINT32__;
                break;
            case KFDataDefine::DataTypeInt64:
                kfdatasetting->_int_max_value = __MAX_INT64__;
                break;
            case KFDataDefine::DataTypeUInt64:
                kfdatasetting->_int_max_value = __MAX_UINT64__;
                break;
            }

            kfdatasetting->_contain_class = xmlnode.ReadString( "containclass", true );
            if ( !kfdatasetting->_contain_class.empty() )
            {
                auto type = KFDataDefine::ConvertDataType( kfdatasetting->_contain_class );
                if ( type != KFDataDefine::DataTypeUnknow )
                {
                    kfdatasetting->_logic_type = type;
                }
                else
                {
                    kfdatasetting->_logic_type = KFDataDefine::DataTypeObject;
                }
            }

            kfdatasetting->_delay_save_time = xmlnode.ReadUInt32( "savetime", true );
            kfdatasetting->_delete_type = xmlnode.ReadUInt32( "deletetype", true );
            kfdatasetting->_str_init_value = xmlnode.ReadString( "initvalue", true );
            kfdatasetting->_str_min_value = xmlnode.ReadString( "minvalue", true );
            kfdatasetting->_str_max_value = xmlnode.ReadString( "maxvalue", true );
            kfdatasetting->_str_run_param = xmlnode.ReadString( "runparam", true );
            kfdatasetting->_str_max_capacity = xmlnode.ReadString( "capacity", true );
            kfdatasetting->_key_name = xmlnode.ReadString( "keyname", true );
            kfdatasetting->_config_key_name = xmlnode.ReadString( "configkeyname", true );
            kfdatasetting->_value_key_name = xmlnode.ReadString( "valuekeyname", true );
            kfdatasetting->_lua_file = xmlnode.ReadString( "luafile", true );
            kfdatasetting->_add_function = xmlnode.ReadString( "addfunction", true );
            kfdatasetting->_update_function = xmlnode.ReadString( "updatefunction", true );
            kfdatasetting->_remove_function = xmlnode.ReadString( "removefunction", true );

            kfdatasetting->_delay_online_sync_time = xmlnode.ReadUInt32( "delay", true );
            if ( kfdatasetting->_delay_online_sync_time > 0u )
            {
                kfsetting->_online_sync_time.insert( kfdatasetting->_delay_online_sync_time );
            }

            for ( auto& iter : _data_mask_list )
            {
                if ( xmlnode.ReadString( iter.first.c_str(), true ) == "1" )
                {
                    KFUtility::AddBitMask< uint32 >( kfdatasetting->_data_mask, iter.second );
                }
            }
        }
    };
}

#endif