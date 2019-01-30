#ifndef __KF_COMPOUND_CONFIG_H__
#define __KF_COMPOUND_CONFIG_H__

#include "KFrame.h"
#include "KFCore/KFElement.h"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    /////////////////////////////////////////////////////////////////////////////////
    class KFCompoundSetting
    {
    public:
        // 属性名字
        std::string _data_name;

        // 属性id
        uint32 _key;

        // 合成的属性
        KFElements _compound_data;

        // 花费材料
        KFElements _cost_data;
    };

    ////////////////////////////////////////////////////////////////////////////////////
    class KFCompoundConfig : public KFConfig, public KFSingleton< KFCompoundConfig >
    {
    public:
        KFCompoundConfig() = default;
        ~KFCompoundConfig() = default;

        bool LoadConfig();

        // 查找配置
        const KFCompoundSetting* FindCompoundSetting( const std::string& dataname, uint32 id );

    public:
        // 称号列表
        typedef std::pair< std::string, uint32 > CompoundKey;
        KFMap< CompoundKey, const CompoundKey&, KFCompoundSetting > _compound_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_compound_config = KFCompoundConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif