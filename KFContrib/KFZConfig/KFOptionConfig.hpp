#ifndef __KF_OPTION_CONFIG_H__
#define __KF_OPTION_CONFIG_H__

#include "KFZConfig/KFConfig.h"
#include "KFZConfig/KFOptionSetting.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFOptionConfig : public KFConfigT< KFOptionSetting >, public KFInstance< KFOptionConfig >
    {
    public:
        KFOptionConfig()
        {
            _file_name = "option";
        }

        // 查找配置选项
        const KFOptionSetting* FindOption( const std::string& name, const std::string& key ) const;
    protected:
        // 创建配置
        KFOptionSetting* CreateSetting( KFNode& xmlnode );

        // 读取配置
        virtual void ReadSetting( KFNode& xmlnode, KFOptionSetting* kfsetting );

    public:
        // 选项配置列表
        typedef std::pair< std::string, std::string > OptionKey;
        KFMap< OptionKey, const OptionKey&, KFOptionSetting > _option_list;
    };
}

#endif