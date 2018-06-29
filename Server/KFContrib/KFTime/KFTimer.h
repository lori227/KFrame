#ifndef __KF_TIMER_H__
#define __KF_TIMER_H__

/************************************************************************

						游戏定时器

************************************************************************/
#include "KFClock.h"
#include "KFDate.h"

namespace KFrame
{
    template< class T >
    class KFTimer
    {
    public:
        KFTimer() : _starttime( 0 ), _keeptime( 0 )
        {

        }

        // 计时器是否已经启动
        bool IsStart() const {
            return _starttime != 0;
        }

        // 检测是否启动, 并开启下一次
        bool CheckStart( uint64 nowtime )
        {
            if ( !IsStart() )
            {
                return false;
            }

            NextTimer( nowtime );
            return true;
        }

        // 检测是否启动, 并停止定时器
        bool CheckStart()
        {
            if ( !IsStart() )
            {
                return false;
            }

            StopTimer();
            return true;
        }

        bool CheckStop()
        {
            DoneTimer();

            return !IsStart();
        }

        // 计时器启动
        void StartTimer( uint64 starttime, uint64 keeptime )
        {
            if ( _starttime == 0 || _keeptime == 0 )
            {
                SetStartTime( starttime );
                SetKeepTime( keeptime );
            }
        }

        // 下一次计时
        void NextTimer( uint64 starttime ) {
            SetStartTime( starttime );
        }

        // 计时器停止
        void StopTimer() {
            _starttime = 0;
        }

        // 设置计时器间隔时间
        uint64 GetStartTime() const {
            return _starttime;
        }
        uint64 GetSpaceTime() const {
            return _keeptime;
        }

        void SetKeepTime( uint64 keeptime ) {
            _keeptime = keeptime;
        }
        void AddKeepTime( uint64 keeptime ) {
            _keeptime += keeptime;
        }

        // 获得定时器剩余时间
        uint64 GetLeftKeepTime( uint64 nowtime ) const
        {
            return T::GetLeftTime( nowtime, _starttime, _keeptime );
        }

        // 是否计时完成( 只计时一次 )
        bool DoneTimer()
        {
            bool done = DoneTimer_();
            if ( done )
            {
                StopTimer();
            }

            return done;
        }

        // 是否计时完成
        bool DoneTimer( uint64 nowtime, bool next )
        {
            bool done = DoneTimer_( nowtime );
            if ( done )
            {
                if ( next )
                {
                    NextTimer( nowtime );
                }
                else
                {
                    StopTimer();
                }
            }

            return done;
        }

    private:
        // 设置开始时间
        void SetStartTime( uint64 starttime )
        {
            if ( starttime == 0 )
            {
                starttime = 1;
            } // 保证开始时间不为0

            _starttime = starttime;
        }

        // 是否计时完成
        bool DoneTimer_()
        {
            if ( _starttime == 0 )
            {
                return false;
            }

            return T::CheckPassTime( _starttime, _keeptime );
        }

        bool DoneTimer_( uint64 nowtime )
        {
            if ( _starttime == 0 )
            {
                return false;
            }

            return T::CheckPassTime( nowtime, _starttime, _keeptime );
        }

    private:
        // 开始时间( 0 表示还未开始 )
        uint64 _starttime;

        // 间隔时间
        uint64 _keeptime;
    };

    typedef KFTimer< KFClock > KFClockTimer;    // 游戏时间定时器
    typedef KFTimer< KFDate > KFDateTimer;		// 现实时间定时器
}
#endif
