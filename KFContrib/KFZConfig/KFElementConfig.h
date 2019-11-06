#ifndef __KF_ELEMENT_CONFIG_H__
#define __KF_ELEMENT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    class KFElementSetting : public KFStrSetting
    {
    public:
        // 类型
        uint32 _type;

        // 格式化字串( 顺序: name value id )
        std::string _element_template;
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    class KFElementConfig : public KFConfigT< KFElementSetting >, public KFInstance< KFElementConfig >
    {
    public:
        KFElementConfig()
        {
            _file_name = "element";
        }

        // 查找配置
        const KFElementSetting* FindElementSetting( const std::string& name )
        {
            static const std::string _default = "default";
            auto kfsetting = FindSetting( name );
            if ( kfsetting == nullptr )
            {
                kfsetting = FindSetting( _default );
            }

            return kfsetting;
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFElementSetting* kfsetting )
        {
            kfsetting->_type = xmlnode.GetUInt32( "Type" );
            kfsetting->_element_template = xmlnode.GetString( "Template" );
        }
    };
}

#endif