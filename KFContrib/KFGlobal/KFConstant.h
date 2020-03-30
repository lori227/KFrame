#ifndef __KF_CONSTANT_H__
#define __KF_CONSTANT_H__

#include "KFInclude.h"

namespace KFrame
{
    class KFConstant
    {
    public:
        // 整型数值
        uint32 _uint32_value = 0;

        // 浮点数值
        double _double_value = 0.0f;

        // 字符串数值
        std::string _str_value;
    };
}

#endif
