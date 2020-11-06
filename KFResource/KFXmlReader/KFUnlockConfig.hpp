#ifndef __KF_UNLOCK_CONFIG_H__
#define __KF_UNLOCK_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFUnlockSetting : public KFIntSetting
    {
    public:
        // 属性
        std::string _data_name;

        // key
        uint32 _data_id;

        // 数值
        uint32 _data_value;

        // 子属性
        std::string _child_name;

        // 完成条件
        KFConditionGroup _condition_group;
    };

    class KFUnlockConfig : public KFConfigT< KFUnlockSetting >, public KFInstance< KFUnlockConfig >
    {
    public:
        KFUnlockConfig()
        {
            _file_name = "unlock";
        }

    protected:

        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFUnlockSetting* kfsetting )
        {
            kfsetting->_data_name = xmlnode.ReadString( "DataName" );
            kfsetting->_data_id = xmlnode.ReadUInt32( "DataKey" );
            kfsetting->_data_value = xmlnode.ReadUInt32( "DataValue" );
            kfsetting->_child_name = xmlnode.ReadString( "ChildName" );
            kfsetting->_condition_group = xmlnode.ReadConditionGroup( "UnLockCondition" );
        }
    };
}

#endif