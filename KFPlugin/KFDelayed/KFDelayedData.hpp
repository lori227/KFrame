#ifndef __KF_DELAYED_DATA_H__
#define __KF_DELAYED_DATA_H__

#include "KFrame.h"
#include "KFDelayedInterface.h"

namespace KFrame
{
    namespace KFDelayedEnum
    {
        enum MyEnum
        {
            Loop = 1,		// 循环
            Once = 2,		// 一次
        };
    }

    class KFDelayedData
    {
    public:
        KFDelayedData( const int8* data, uint32 size )
        {
            if ( data == nullptr || size == 0 )
            {
                return;
            }

            _size = size;
            _data = __KF_MALLOC__( int8, size );
            memcpy( _data, data, size );
        }

        ~KFDelayedData()
        {
            if ( _data != nullptr )
            {
                __KF_FREE__( int8, _data );
            }
        }

    public:
        // 设定的执行时间
        KFTimeData _time_data;

        // 下一次执行的时间
        uint64 _next_execute_time = 0;

        // 循环类型
        uint32 _loop_type = KFDelayedEnum::Once;

        // 回调函数
        KFFunction< KFDelayedFunction > _function;

        // 回调参数
        uint64 _object_id = 0u;
        int8* _data = nullptr;
        uint32 _size = 0u;
    };
}

#endif