#ifndef __KF_TIMER_DATA_H__
#define __KF_TIMER_DATA_H__

#include "KFTimerInterface.h"

namespace KFrame
{
    //////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////
    class KFTimerData
    {
    public:
        KFTimerData();

    public:
        //
        std::string _module;

        // id
        uint64 _id;

        // 类型
        uint32 _type;

        // 次数
        uint32 _count;

        // 时间间隔
        uint32 _interval;

        // 延迟时间
        uint32 _delay;

        // 时间轮
        uint32 _rotation;

        // 槽索引
        uint32 _slot;

        // 回调函数
        KFTimerFunction _function;

        // 链表
        KFTimerData* _prev;
        KFTimerData* _next;
    };
}






#endif