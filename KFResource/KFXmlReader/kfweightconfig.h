#ifndef __KF_WEIGHT_CONFIG_H__
#define __KF_WEIGHT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFWeightSetting : public KFIntSetting
    {
    public:
        // 权重列表
        KFWeightList< KFWeight > _weight_list;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFWeightConfig : public KFConfigT< KFWeightSetting >, public KFInstance< KFWeightConfig >
    {
    public:
        KFWeightConfig()
        {
            _file_name = "weight";
        }

        // 查找权重池
        inline const KFWeightList< KFWeight >* FindWeightPool( uint32 id )
        {
            auto kfsetting = FindSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }

            return &kfsetting->_weight_list;
        }
    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFWeightSetting* kfsetting )
        {
            auto value = xmlnode.GetUInt32( "Value" );
            auto weight = xmlnode.GetUInt32( "Weight" );
            kfsetting->_weight_list.Create( value, weight );
        }
    };
}

#endif