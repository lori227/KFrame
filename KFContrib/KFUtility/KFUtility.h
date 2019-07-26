#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "KFInclude.h"
#include "KFDefine.h"

namespace KFrame
{
#define DEFAULT_SPLIT_STRING "|"
#define FUNCTION_EMPTY_STRING ""
#define FUNCTION_RANGE_STRING "-"
#define FUNCTION_LINK_STRING ":"

    class KFUtility
    {
    public:
        // 算术操作
        template< class T = uint64 > static T Operate( uint32 operate, T basevalue, T value );

        // 判断是否满足条件
        template< class T = uint64 > static bool CheckOperate( T basevalue, uint32 operate, T operatevalue );

        // 转换成字符串
        template< class T = uint64 > static std::string ToString( const T& value );

        // 转换成数值
        template< class T = uint64 > static T ToValue( const char* srcstring );
        template< class T = uint64 > static T ToValue( const std::string& srcstring );

        // 切割字符串
        template< class T = uint64 > static T SplitValue( std::string& srcstring, std::string split );

        // 标记
        template< class T = uint32 > static bool HaveBitMask( T value, T bitmask );
        template< class T = uint32 > static void ClearBitMask( T& value, T bitmask );
        template< class T = uint32 > static void AddBitMask( T& value, T bitmask );

        template< class T > static T SplitList( std::string& srcstring, std::string split );
        template< class T > static T SplitSet( std::string& srcstring, std::string split );
        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 判断float double 是否为0
        static bool IsZeroFloat( float value, float epsilon = 1e-6 );
        static bool IsZeroDouble( double value, double epsilon = 1e-15 );
        static bool IsNumber( const std::string& value );

        // 计算两点之间距离
        static uint32 CalcDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety );

        // 判断是否在距离范围内
        static bool CheckInDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety, uint32 distance );

        // 计算hash值
        static uint32 GetHashValue( const std::string& data );

        // 切割字符串
        static std::string SplitString( std::string& srcstring, std::string split );

        // 替换字串
        static void ReplaceString( std::string& srcstring, const std::string& strold, const std::string& strnew );
        static void ReplaceString( std::string& srcstring, const std::string& strold, const VectorString& params );

        // 删除一个字符串中的指定字符串
        static bool DelString( std::string& srcstring, const std::string& delstr, std::string& strnew );

        // 格式化标题
        static std::string FormatTitleText( const std::string& appname, const std::string& apptype, const std::string& appid );

        ////////////////////////////////////////////////////////////////////////////////////////////////
        static std::string FormatConfigFile( const std::string& filename, uint32 channel, uint32 service );

    };
}

#include "KFUtility.inl"

#endif