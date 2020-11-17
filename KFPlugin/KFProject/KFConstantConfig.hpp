#ifndef __KF_CONSTANT_CONFIG_H__
#define __KF_CONSTANT_CONFIG_H__

#include "KFConfig.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFConstantConfig : public KFConfigT< KFIntSetting >, public KFInstance< KFConstantConfig >
    {
    public:
        KFConstantConfig()
        {
            _file_name = "constant";
        }

    protected:
        // 读取配置
        virtual void ReadSetting( KFXmlNode& xmlnode, KFIntSetting* kfsetting )
        {
            auto name = xmlnode.ReadString( "name" );
            auto key = xmlnode.ReadUInt32( "key", true );
            auto value = xmlnode.ReadString( "value", true );
            KFGlobal::Instance()->AddConstant( name, key, value );
        }
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////

}

#endif