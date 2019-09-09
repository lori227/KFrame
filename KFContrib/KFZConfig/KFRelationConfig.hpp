#ifndef __KF_RELATION_CONFIG_H__
#define __KF_RELATION_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFRelationSetting.hpp"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFRelationConfig : public KFConfigT< KFRelationSetting >, public KFInstance< KFRelationConfig >
    {
    public:
        KFRelationConfig()
        {
            _file_name = "relation";
        }

        // 查找关系配置
        const KFRelationSetting* FindRelationSettingByInviteName( const std::string& name );

    protected:
        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFRelationSetting* kfsetting );
    };
}

#endif