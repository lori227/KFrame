#ifndef __KF_CONDITION_OPTION_H__
#define __KF_CONDITION_OPTION_H__

#include "KFDefine.h"

namespace KFrame
{
    // 动态条件id组
    class KFDynamicConditionGroup
    {
    public:
        // 条件判断类型( |, & )
        uint32 _type = 0u;

        // 条件的id列表
        UInt32Vector _id_list;
    };
    typedef std::shared_ptr< KFDynamicConditionGroup > DynamicConditionGroupPtr;
    ////////////////////////////////////////////////////////////

}

#endif