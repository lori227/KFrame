#ifndef __KF_CONDITION_GROUP_H__
#define __KF_CONDITION_GROUP_H__

#include "KFInclude.h"

namespace KFrame
{
    // 条件id组
    class KFConditionGroup
    {
    public:
        // 条件判断类型( |, & )
        uint32 _type = 0u;

        // 条件的id列表
        UInt32Vector _ids;
    };

}

#endif