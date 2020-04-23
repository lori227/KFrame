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
        void SetValue( T minvalue, T maxvalue )
        {
            _min_value = minvalue;
            _max_value = maxvalue;
            if ( _min_value > _max_value )
            {
                _max_value = _min_value;
            }
        }

        void SetValue( std::string strdata )
        {
            auto minvalue = KFUtility::SplitValue< T >( strdata, __RANGE_STRING__ );
            auto maxvalue = KFUtility::SplitValue< T >( strdata, __RANGE_STRING__ );
            SetValue( minvalue, maxvalue );
        }

        // 计算数值
        T CalcValue() const
        {
            return ( T )KFGlobal::Instance()->RandRange( _min_value, _max_value );
        }

        // 是否有效
        bool IsValid() const
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