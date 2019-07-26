#ifndef __KF_SETTING_H__
#define __KF_SETTING_H__

#include "KFInclude.h"

namespace KFrame
{
    ///////////////////////////////////////////////////////////////
    class KFSetting
    {
    public:
        KFSetting() = default;
        virtual ~KFSetting() = default;
    };
    ///////////////////////////////////////////////////////////////
    class KFIntSetting : public KFSetting
    {
    public:
        // 类型
        typedef uint64 Type;
        typedef uint64 ParamType;

    public:
        uint64 _id = 0u;
    };
    ///////////////////////////////////////////////////////////////
    class KFStrSetting : public KFSetting
    {
    public:
        // 类型
        typedef std::string Type;
        typedef const std::string& ParamType;

    public:
        std::string _id;
    };
    ///////////////////////////////////////////////////////////////
}

#endif