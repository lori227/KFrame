#ifndef __KF_PROPERTY_CONFIG_H__
#define __KF_PROPERTY_CONFIG_H__

#include "KFMap.h"
#include "KFSingleton.h"
#include "KFDataSetting.h"
#include "KFClassSetting.h"

namespace KFrame
{
    class KFDataConfig : public KFSingleton< KFDataConfig >
    {
    public:
        KFDataConfig() = default;
        ~KFDataConfig() = default;

        // 获得静态类配置
        const KFClassSetting* FindClassSetting( const std::string& classname ) const;

        // 获得活动配置
        const KFDataSetting* FindDataSetting( const std::string& classname, const std::string& propertyname ) const;

        // 加载
        bool LoadDataConfig( const std::string& file );

    public:
        // 静态类列表
        KFHashMap< std::string, const std::string&, KFClassSetting > _class_setting;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////
    static auto _kf_data_config = KFDataConfig::Instance();
    //////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif