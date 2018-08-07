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
        template< class T = uint32 > static std::string ToString( const T& value );

        // 转换成数值
        template< class T = uint32 > static T ToValue( const char* srcstring );
        template< class T = uint32 > static T ToValue( const std::string& srcstring );

        // 切割字符串
        template< class T = uint32 > static T SplitValue( std::string& srcstring, std::string split );

        // 标记
        template< class T = uint32 > static bool HaveBitMask( T value, T bitmask );
        template< class T = uint32 > static void ClearBitMask( T& value, T bitmask );
        template< class T = uint32 > static void AddBitMask( T& value, T bitmask );

        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////////
        // 获得玩家zoneid
        static uint32 CalcZoneId( uint32 playerid );

        // 计算玩家id
        static uint32 CalcPlayerid( uint32 id, uint32 zoneid );

        // 计算zone服务器id
        static uint32 CalcZoneServerId( uint32 serverid, uint32 zoneid );

        // 计算服务器的zoneid
        static uint32 CalcServerZoneId( uint32 serverid );

        // 判断float double 是否为0
        static bool IsZeroFloat( float value, float epsilon = 1e-6 );
        static bool IsZeroDouble( double value, double epsilon = 1e-15 );

        // 计算两点之间距离
        static uint32 CalcDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety );

        // 判断是否在距离范围内
        static bool CheckInDistance( uint32 nowx, uint32 nowy, uint32 targetx, uint32 targety, uint32 distance );

        // 切割字符串
        static std::string SplitString( std::string& srcstring, std::string split );

        // 替换字串
        static void ReplaceString( std::string& srcstring, const std::string& strold, const std::string& strnew );
        static void ReplaceString( std::string& srcstring, const std::string& strold, const VectorString& params );

        // 生产guid
        static uint64 Make64Guid( uint32 dataid );

        // 格式化标题
        static std::string FormatTitleText( const std::string& appname, const std::string& apptype, uint32 appid );

    };
}

#include "KFUtility.inl"

#endif