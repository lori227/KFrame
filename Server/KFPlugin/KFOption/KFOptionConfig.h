#ifndef __KF_OPTION_CONFIG_H__
#define __KF_OPTION_CONFIG_H__

#include "KFrame.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////
    class KFOptionConfig : public KFConfig, public KFSingleton< KFOptionConfig >
    {
    public:
        KFOptionConfig();
        ~KFOptionConfig();

        // 加载配置
        bool LoadConfig( const char* file );

        // 查找配置选项
        const std::string& FindOption( const std::string& name, uint32 key ) const;

    public:
        // 选项配置列表
        typedef std::pair< std::string, uint32 > OptionKey;
        std::map< OptionKey, std::string > _option_list;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_option_config = KFOptionConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif