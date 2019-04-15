#ifndef __KF_CLASS_SETTING_H__
#define __KF_CLASS_SETTING_H__

#include "KFDataSetting.h"

namespace KFrame
{
    // 对象类定义
    ////////////////////////////////////////////////////////////////////////////////////////
    class KFClassSetting
    {
    public:
        KFClassSetting() = default;
        ~KFClassSetting() = default;

        const KFDataSetting* FindDataSetting( const std::string& name ) const;
        bool IsChildData( const std::string& name ) const;

    public:
        // 名称
        std::string _class;

        // 属性列表
        KFHashMap< std::string, const std::string&, KFDataSetting > _static_data;
    };
}
#endif
