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

        // 是否是配置文件
        virtual bool IsFile( const std::string& configfile, const std::string& file );

        // 加载完成
        virtual void LoadComplete( const std::string& file );
    protected:
        // 创建配置
        KFOption* CreateSetting( KFNode& xmlnode );

        // 读取配置
        void ReadSetting( KFNode& xmlnode, KFOption* kfsetting );

    public:
        // 选项配置列表
        typedef std::pair< std::string, std::string > OptionKey;
        KFMap< OptionKey, const OptionKey&, KFOption > _option_list;

        // 文件列表
        std::set< std::string > _file_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_option_config = KFOptionConfig::Instance( "option.xml", false );
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif