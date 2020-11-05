#ifndef __KF_RESET_CONFIG_H__
#define __KF_RESET_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFResetData
    {
    public:
        std::string _function_name;		// 调用的函数名

        std::string _parent_name;		// 父属性变量名称
        uint32 _key = 0u;				// 属性的变量id
        std::string _data_name;			// 属性变量名称
        uint32 _operate = KFEnum::Set;	// 操作的类型
        uint32 _value = 0u;				// 操作的初始值
    };

    class KFResetSetting : public KFIntSetting
    {
    public:
        // 重置的数据列表
        std::vector< KFResetData > _reset_data_list;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFResetConfig : public KFConfigT< KFResetSetting >, public KFInstance< KFResetConfig >
    {
    public:
        KFResetConfig()
        {
            _file_name = "reset";
        }

    protected:
        // 创建配置
        KFResetSetting* CreateSetting( KFNode& xmlnode )
        {
            return _settings.Create( 0 );
        }

        virtual void ClearSetting()
        {
            _reset_settings.Clear();
            KFConfigT< KFResetSetting >::ClearSetting();
        }

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFResetSetting* kfsetting )
        {
            auto timeid = xmlnode.GetUInt32( "TimeId" );
            auto resetsetting = _reset_settings.Create( timeid );

            // 重置信息
            KFResetData resetdata;
            resetdata._function_name = xmlnode.GetString( "FunctionName", true );
            resetdata._parent_name = xmlnode.GetString( "ParentName", true );
            resetdata._key = xmlnode.GetUInt32( "Key", true );
            resetdata._data_name = xmlnode.GetString( "DataName" );
            resetdata._operate = xmlnode.GetUInt32( "Operate" );
            resetdata._value = xmlnode.GetUInt32( "Value", true );
            resetsetting->_reset_data_list.push_back( resetdata );
        }

    public:
        KFMap<uint32, uint32, KFResetSetting> _reset_settings;
    };
}

#endif