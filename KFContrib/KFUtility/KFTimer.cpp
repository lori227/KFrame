#include "KFTimer.h"
#include "KFGlobal/KFGlobal.h"

namespace KFrame
{
    void KFTimer::StartCount( uint32 interval, uint32 count /* = 1u */, bool immediately /* = false */ )
    {
        _done_count = 0u;
        _max_count = count;
        _interval = interval;

        if ( immediately )
        {
            _start_time = 1u;
        }
        else
        {
            _start_time = KFGlobal::Instance()->_game_time;
        }
    }

    void KFTimer::StartLoop( uint32 interval, bool immediately /* = false */ )
    {
        StartCount( interval, 0, immediately );
    }

    bool KFTimer::IsStart() const
    {
        return _start_time != 0u;
    }

    bool KFTimer::IsFinish() const
    {
        if ( _max_count == 0u )
        {
            return false;
        }

        return _done_count >= _max_count;
    }

    void KFTimer::StopTimer()
    {
        _start_time = 0u;
        _interval = 0u;
        _max_count = 0u;
        _done_count = 0u;
    }

    uint32 KFTimer::GetInterval() const
    {
        return _interval;
    }

    void KFTimer::SetInterval( uint32 interval )
    {
        _interval = interval;
    }

    void KFTimer::AddInterval( uint32 interval )
    {
        _interval += interval;
    }

    uint32 KFTimer::GetLeftTime() const
    {
        auto now_time = KFGlobal::Instance()->_game_time;
        auto pass_time = now_time - _start_time;
        return _interval - static_cast<uint32>( pass_time );
    }


    // 是否计时完成( 只计时一次 )
    bool KFTimer::DoneTimer()
    {
        if ( _start_time == 0u || _interval == 0u )
        {
            return false;
        }

        auto now_time = KFGlobal::Instance()->_game_time;
        if ( now_time < _start_time + _interval )
        {
            return false;
        }

        if ( _max_count != 0u )
        {
            ++_done_count;
            if ( _done_count == _max_count )
            {
                _start_time = 0u;
                _interval = 0u;
            }
        }

        return true;
    }
}
