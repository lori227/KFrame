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
        // 合成id
        uint32 _id = 0u;

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
    class KFCompoundConfig : public KFConfig, public KFSingleton< KFCompoundConfig >
    {
    public:
        KFCompoundConfig() = default;
        ~KFCompoundConfig() = default;

        bool LoadConfig( const std::string& file );

        // 查找配置
        const KFCompoundSetting* FindCompoundSetting( uint32 id );

    public:
        // 称号列表
        KFMap< uint32, uint32, KFCompoundSetting > _compound_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_compound_config = KFCompoundConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif