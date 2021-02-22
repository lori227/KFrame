#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "KFDefine.h"

namespace KFrame
{
    class KFUtility
    {
    public:
        // 算术操作
        template<class T = uint64> static T Operate( T base_value, uint32 operate, T value );

        // 判断是否满足条件
        template<class T = uint64> static bool CheckOperate( T base_value, uint32 operate, T operate_value );
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 转换成字符串
        template<class T = uint64> static std::string ToString( const T& value );

        // 转换成数值
        template<class T = uint64> static T ToValue( const char* source );
        template<class T = uint64> static T ToValue( const std::string& source );
        ////////////////////////////////////////////////////////////////////////////////////////////

        // 切割字符串
        template<class T = uint64> static T SplitValue( std::string& source, const std::string& split );

        template<class T> static void SplitSet( std::string source, const std::string& split, T& out_set );
        template<class T> static void SplitList( std::string source, const std::string& split, T& out_list );

        template<class T> static bool ParseArraySet( const std::string& source, T& out_set );
        template<class T> static bool ParseArrayList( const std::string& source, T& out_list );
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 标记
        template< class T = uint32 > static bool HaveBitMask( T value, T bit_mask );
        template< class T = uint32 > static void ClearBitMask( T& value, T bit_mask );
        template< class T = uint32 > static void AddBitMask( T& value, T bit_mask );
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 判断float double 是否相等
        static bool IsFloatEqual( float first, float second );
        static bool IsDoubleEqual( double first, double second );

        // 简单判断是不是数字
        static bool IsNumber( const std::string& value );

        // 计算两点之间距离
        static uint32 CalcDistance( uint32 now_x, uint32 now_y, uint32 target_x, uint32 target_y );

        // 判断是否在距离范围内
        static bool CheckInDistance( uint32 now_x, uint32 now_y, uint32 target_x, uint32 target_y, uint32 distance );

        // 计算hash值
        static uint32 GetHashValue( const std::string& data );

        // 切割字符串
        static std::string SplitString( std::string& source, const std::string& split );

        // 替换字串
        static void ReplaceString( std::string& source, const std::string& str_old, const std::string& str_new );
        static void ReplaceString( std::string& source, const std::string& str_old, const StringVector& params );

        // 删除一个字符串中的指定字符串
        static bool DelString( std::string& source, const std::string& str_del, std::string& str_new );

        // 格式化标题
        static std::string FormatTitleText( const std::string& app_name, const std::string& app_type, const std::string& app_id );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string FormatConfigFile( const std::string& filename, uint32 channel, uint32 service );
        ////////////////////////////////////////////////////////////////////////////////////////////////
        // 获得map中最大的值
        static uint32 GetMaxMapValue( const UInt32Map& map_values, uint32 value );

        // 按权重随机map
        static uint32 RandMapValue( const UInt32Map& map_values, uint32 total_weight, uint32 rand );

        // 随机一个值
        static uint32 RandVectorValue( const UInt32Vector& vector_values, const UInt32Set& exclude_list );

    };
}

#include "KFUtility.inl"

#endif