#ifndef __KF_COMPOUND_CONFIG_H__
#define __KF_COMPOUND_CONFIG_H__

#include "KFConfig.h"
#include "KFElementConfig.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFCompoundSetting : public KFIntSetting
    {
    public:
        // 属性名字
        std::string _data_name;

        // 属性id
        uint32 _key = 0u;

        // 合成的属性
        KFElements _compound_data;

        // 花费材料
        KFElements _cost_data;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFCompoundConfig : public KFConfigT< KFCompoundSetting >, public KFInstance< KFCompoundConfig >
    {
    public:
        KFCompoundConfig()
        {
            _file_name = "compound";
        }

        virtual void LoadAllComplete()
        {
            for ( auto& iter : _settings._objects )
            {
                auto kfsetting = iter.second;

                KFElementConfig::Instance()->ParseElement( kfsetting->_cost_data, __FILE__, __LINE__ );
                KFElementConfig::Instance()->ParseElement( kfsetting->_compound_data, __FILE__, __LINE__ );
            }
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFCompoundSetting* kfsetting )
        {
            kfsetting->_key = xmlnode.ReadUInt32( "DataKey" );
            kfsetting->_data_name = xmlnode.ReadString( "DataName" );

            kfsetting->_cost_data._str_parse = xmlnode.ReadString( "CostData" );
            kfsetting->_compound_data._str_parse = xmlnode.ReadString( "CompoundData" );
        }
    };
}

#endif