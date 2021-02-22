#ifndef __KF_STATIC_CONDITION_ANALYSIS_H__
#define __KF_STATIC_CONDITION_ANALYSIS_H__

#include "KFStaticCondition.h"

namespace KFrame
{
    class KFStaticConditionAnalysis
    {
    public:
        // 重新格式化
        static void Cleanup( std::string& value );

        // 是否是数字
        static bool IsInteger( int8 value );

        // 是否是字母
        static bool IsLetter( int8 value );

        // 是否是属性key
        static bool IsDataKey( int8 value );

        // 获得操作计算符
        static bool IsOperatorType( int8 value );
        static uint32 GetOperatorType( int8 value );

        // 获得判断结算福
        static uint32 GetCheckType( const std::string& value );

        // 获得界符
        static bool IsDelimiter( int8 value );
        static int8 GetRightDelimiter( int8 value );

        // 获得连接符
        static bool IsLinkType( int8 value );
        static uint32 GetLinkType( int8 value );

        // 解析
        static StaticConditionsPtr Parse( std::string& condition );

        // 解析条件
        static std::tuple<std::shared_ptr<KFStaticCondition>, uint32, bool> ParseCondition( const int8* data, uint32 size, uint32 start_pos, uint32& end_pos );

        // 读取条件
        static uint32 ReadCondition( const int8* data, uint32 size, uint32 start_pos, uint32& end_pos );

        // 读取判断类型
        static std::tuple<uint32, uint32, uint32> ReadCheckType( const int8* data, uint32 size, uint32 start_pos );

        // 解析表达式
        static std::tuple<std::shared_ptr<KFStaticConditionExpression>, bool> ParseExpression( const int8* data, uint32 size, uint32 start_pos );

        // 解析数字
        static std::shared_ptr<KFStaticConditionAbstract> ParseConstant( const int8* data, uint32& size, uint32& start_pos );

        // 解析属性名
        static std::shared_ptr<KFStaticConditionAbstract> ParseVariable( const int8* data, uint32& size, uint32& start_pos );

        // 读取Integer
        static uint32 ReadInteger( const int8* data, uint32& size, uint32& start_pos );

        // 读取变量
        static std::string ReadLetter( const int8* data, uint32& size, uint32& start_pos );
    };
}


#endif
