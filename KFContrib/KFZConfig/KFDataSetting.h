#ifndef __KF_DATA_SETTING_H__
#define __KF_DATA_SETTING_H__

#include "KFrame.h"
#include "KFSetting.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////
    namespace KFDataDefine
    {
        enum MyEnum
        {
            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////
            // 数据类型
            DataTypeUnknow = 0,
            DataTypeInt32 = 1,
            DataTypeUInt32 = 2,
            DataTypeInt64 = 3,
            DataTypeUInt64 = 4,
            DataTypeDouble = 5,
            DataTypeString = 6,
            DataTypeVector3D = 7,
            DataTypeObject = 8,
            DataTypeRecord = 9,
            DataTypeArray = 10,
            ////////////////////////////////////////////////////////////////////////////

            DataMaskNull = 0,				// 无效值
            DataMaskSync = 1 << 1,			// 更新同步玩家
            DataMaskView = 1 << 2,			// 同步给附近玩家
            DataMaskSave = 1 << 3,			// 是否保存数据
            DataMaskMultiple = 1 << 4,		// 是否支持倍数
            DataMaskShow = 1 << 5,			// 是否显示给客户端
            DataMaskRequest = 1 << 6,		// 是否请求才同步客户端
            DataMaskAddCall = 1 << 7,		// 是否添加回调
            DataMaskRemoveCall = 1 << 8,	// 是否删除回调
            DataMaskUpdataCall = 1 << 9,	// 是否更新回调
            DataMaskShield = 1 << 10,		// 是否屏蔽属性(项目可设置这个属性来屏蔽框架创建)
            DataMaskClient = 1 << 11,		// 是否客户端属性(客户端可随意修改)

            DataMaskBasic = 1 << 20,		// 基础属性
            DataMaskRelation = 1 << 21,		// 关系属性
            DataMaskTeam = 1 << 22,			// 队伍属性
            DataMaskGuild = 1 << 23,		// 帮派属性
            DataMaskRank = 1 << 24,			// 排行榜属性
            ////////////////////////////////////////////////////////////////////////////
            DataDeleteNone  = 0,			// 需要保存
            DataDeleteSave = 1,				// 保存数据库
            DataDeleteRemove = 2,			// 删除数据库
            ////////////////////////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////////////////////
        };

        static const StringUInt32 _data_type_list =
        {
            {"int32", DataTypeInt32},
            {"uint32", DataTypeUInt32},
            {"int64", DataTypeInt64},
            {"uint64", DataTypeUInt64},
            {"double", DataTypeDouble},
            {"string", DataTypeString},
            {"vector3d", DataTypeVector3D},
            {"object", DataTypeObject},
            {"record", DataTypeRecord},
            {"array", DataTypeArray},
        };

        static uint32 ConvertDataType( const std::string& strtype )
        {
            auto iter = _data_type_list.find( strtype );
            if ( iter == _data_type_list.end() )
            {
                return DataTypeUnknow;
            }

            return iter->second;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    class KFClassSetting;
    class KFDataSetting
    {
    public:
        // 属性标记
        inline bool HaveMask( uint32 mask ) const
        {
            if ( mask == 0u )
            {
                return true;
            }

            return KFUtility::HaveBitMask( _data_mask, mask );
        }

    public:
        // 属性名字
        std::string _name;

        // 逻辑名字
        std::string _logic_name;

        // 属性类型
        uint32 _type = 0u;
        uint32 _logic_type = 0u;

        // 包含类型
        std::string _contain_class;
        const KFClassSetting* _class_setting = nullptr;

        // 最大容量
        std::string _str_max_capacity;
        uint32 _int_max_capacity = __MAX_UINT32__;

        // 初始值
        std::string _str_init_value;
        uint32 _int_init_value = 0u;

        // 最小值
        std::string _str_min_value;
        uint32 _int_min_value = 0u;

        // 最大值
        std::string _str_max_value;
        uint64 _int_max_value = __MAX_UINT64__;

        // 运行时参数
        std::string _str_run_param;
        uint32 _int_run_param = 0u;

        // 属性标记
        uint32 _data_mask = 0u;

        // 延迟保存时间
        uint32 _delay_save_time = 0u;

        // 延迟发送给客户端( 0, 表示理解发送, 只在玩家上线时有效, 保证客户端最快速度进入主界面 )
        uint32 _delay_online_sync_time = 0u;

        // 删除时操作
        uint32 _delete_type = 0u;

        // 索引字段
        std::string _key_name;

        // 配置索引值
        std::string _config_key_name;

        // 数值属性字段
        std::string _value_key_name;

        // 脚本文件
        std::string _lua_file;

        // 脚本函数
        std::string _update_function;
        std::string _add_function;
        std::string _remove_function;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // 对象类定义
    class KFClassSetting : public KFStrSetting
    {
    public:
        // 查找配置
        inline const KFDataSetting* FindSetting( const std::string& name ) const
        {
            return _static_data.Find( name );
        }

        // 是否是子节点
        inline bool IsChildData( const std::string& name ) const
        {
            return _static_data.Find( name ) != nullptr;
        }

        // 查找属性
        inline const KFDataSetting* TraversalSetting( const std::string& name ) const
        {
            for ( auto& iter : _static_data._objects )
            {
                auto kfdatassetting = iter.second;
                if ( kfdatassetting->_name == name )
                {
                    return kfdatassetting;
                }

                if ( kfdatassetting->_class_setting != nullptr )
                {
                    auto kfchildsetting = kfdatassetting->_class_setting->TraversalSetting( name );
                    if ( kfchildsetting != nullptr )
                    {
                        return kfchildsetting;
                    }
                }
            }

            return nullptr;
        }

        // 查找属性名
        inline const KFDataSetting* FindSettingByLogicName( const std::string& name ) const
        {
            auto iter = _logic_name_list.find( name );
            if ( iter == _logic_name_list.end() )
            {
                return nullptr;
            }

            return FindSetting( iter->second );
        }

    public:
        // 属性列表
        KFHashMap< std::string, const std::string&, KFDataSetting > _static_data;

        // 逻辑名字对应的属性名
        StringMap _logic_name_list;
    };
    ////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////
}
#endif
