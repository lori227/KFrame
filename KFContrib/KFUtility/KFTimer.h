#ifndef __KF_TIMER_H__
#define __KF_TIMER_H__

/************************************************************************

						游戏定时器

************************************************************************/
#include "KFInclude.h"

namespace KFrame
{
    class KFTimer
    {
    public:
        // 启动循环定时器
        // interval 间隔时间
        // immediately 是否立即执行( 立即指的是下一帧 )
        void StartLoop( uint32 interval, bool immediately = false );

        // 启动数量定时器
        // interval 间隔时间
        // count 指定执行次数
        // immediately 是否立即执行( 立即指的是下一帧 )
        void StartCount( uint32 interval, uint32 count = 1u, bool immediately = false );

        // 计时器是否已经启动
        bool IsStart() const;

        // 是否已经结束( 只对次数定时器有效 )
        bool IsFinish() const;

        // 计时器停止
        void StopTimer();

        // 获得间隔时间
        uint32 GetInterval() const;

        // 设置间隔时间
        void SetInterval( uint32 interval );

        // 添加间隔时间
        void AddInterval( uint32 interval );

        // 获得定时器剩余时间
        uint32 GetLeftTime() const;

        // 定时器是否完成
        bool DoneTimer();

    private:
        // 开始时间( 0 表示还未开始 )
        uint64 _start_time = 0u;

        // 间隔时间
        uint32 _interval = 0u;

        // 设定执行次数
        uint32 _max_count = 0u;

        // 完成次数
        uint32 _done_count = 0u;
    };
}
#endif
