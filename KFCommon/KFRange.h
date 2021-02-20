#ifndef __KF_RANGE_H__
#define __KF_RANGE_H__

#include "KFGlobal/KFGlobal.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    template< class T = uint32 >
    class KFRange
    {
    public:
        inline void SetValue( T min_value, T max_value )
        {
            _min_value = min_value;
            _max_value = max_value;
            if ( _min_value > _max_value )
            {
                _max_value = _min_value;
            }
        }

        inline void SetValue( std::string data )
        {
            auto min_value = KFUtility::SplitValue<T>( data, __RANGE_STRING__ );
            auto max_value = KFUtility::SplitValue<T>( data, __RANGE_STRING__ );
            SetValue( min_value, max_value );
        }

        // 计算数值
        inline T CalcValue() const
        {
            return ( T )KFGlobal::Instance()->RandRange( _min_value, _max_value );
        }

        // 是否有效
        inline bool IsValid() const
        {
            return _max_value > 0;
        }

    public:
        T _min_value = 0;
        T _max_value = 0;
    };
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<>
    inline double KFRange<double>::CalcValue() const
    {
        return KFGlobal::Instance()->RandDouble( _min_value, _max_value );
    }

    template<>
    inline float KFRange<float>::CalcValue() const
    {
        return ( float )KFGlobal::Instance()->RandDouble( _min_value, _max_value );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#endif