#ifndef __KF_ELEMENT_CONFIG_H__
#define __KF_ELEMENT_CONFIG_H__

#include "KFConfig.h"
#include "KFElementSetting.h"

namespace KFrame
{
    class KFElementConfig : public KFStrConfigT< KFElementSetting >, public KFInstance< KFElementConfig >
    {
    public:
        KFElementConfig()
        {
            _file_name = "element";
        }

        // 查找配置
        const KFElementSetting* FindElementSetting( const std::string& name );

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFElementSetting* kfsetting );
    };
}

#endif