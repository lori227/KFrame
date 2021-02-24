#ifndef __KF_SETTING_H__
#define __KF_SETTING_H__

#include "KFDefine.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    class KFSetting
    {
    public:
        KFSetting() = default;
        virtual ~KFSetting() = default;
    public:
        // 文件名
        std::string _file_path;

        // 行数
        uint32 _row = 0u;
    };
    ///////////////////////////////////////////////////////////////
    class KFIntSetting : public KFSetting
    {
    public:
        // 类型
        typedef uint64 Type;
    public:
        uint64 _id = 0u;
    };
    ///////////////////////////////////////////////////////////////
    class KFStrSetting : public KFSetting
    {
    public:
        // 类型
        typedef std::string Type;

    public:
        std::string _id;
    };
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    class KFConditionDefineSetting;
    class KFConditionDefineOption
    {
    public:
        std::string _str_condition;
        std::shared_ptr<const KFConditionDefineSetting> _condition_define = nullptr;
    };




}

#endif