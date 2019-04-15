#include "KFTimerData.hpp"

namespace KFrame
{
    KFTimerData::KFTimerData()
    {
        _type = 0;
        _count = 0;
        _id = 0;
        _rotation = 0;
        _delay = 0;
        _slot = 0;
        _interval = 0;
        _prev = nullptr;
        _next = nullptr;
    }
}