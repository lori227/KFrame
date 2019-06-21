#ifndef __KF_OPTION_CONFIG_H__
#define __KF_OPTION_CONFIG_H__

#include "KFOptionInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFOptionConfig : public KFIntConfigT< KFOption >, public KFSingleton< KFOptionConfig >
    {
    public:
        KFOptionConfig( const std::string& file, bool isclear )
            : KFIntConfigT< KFOption >( file, isclear )
        {
        }

        // 查找配置选项
        const KFOption* FindOption( const std::string& name, const std::string& key ) const;

    protected:
        // 创建配置
        KFOption* CreateSetting( KFNode& xmlnode );

        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFOption* kfsetting );

    public:
        // 选项配置列表
        typedef std::pair< std::string, std::string > OptionKey;
        KFMap< OptionKey, const OptionKey&, KFOption > _option_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_option_config = KFOptionConfig::Instance( "option.xml", false );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif