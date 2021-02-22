#include "KFrame.h"
#include "KFStaticAnalysis.hpp"

namespace KFrame
{
    // 操作符列表
    static std::map<int8, uint32> _operator_type =
    {
        {'+', KFEnum::Add},
        {'-', KFEnum::Dec},
        {'*', KFEnum::Mul},
        {'/', KFEnum::Div},
    };

    // 判断符号列表
    static std::map<std::string, uint32> _check_type =
    {
        {">", KFEnum::Greater},
        {"<", KFEnum::Less},
        {"=", KFEnum::Equal},
        {"==", KFEnum::Equal},
        {"!=", KFEnum::UnEqual},
        {">=", KFEnum::GreaterEqual},
        {"<=", KFEnum::LessEqual},
    };

    // 界符
    static std::map<int8, int8> _delimiter =
    {
        {'(', ')'},
        {'{', '}'},
        {'[', ']'},
    };

    // 连接符号
    static std::map<int8, uint32> _link_type =
    {
        {'&', KFEnum::And},
        {'|', KFEnum::Or},
    };

    static int8 _key_type = ':';
    /////////////////////////////////////////////////////////////
    void KFStaticConditionAnalysis::Cleanup( std::string& value )
    {
        // 去除空格
        KFUtility::ReplaceString( value, " ", "" );

        // 转成小写
        std::transform( value.begin(), value.end(), value.begin(), ::tolower );
    }

    bool KFStaticConditionAnalysis::IsInteger( int8 value )
    {
        return ( value >= '0' && value <= '9' );
    }

    bool KFStaticConditionAnalysis::IsLetter( int8 value )
    {
        return ( value >= 'a' && value <= 'z' );
    }

    bool KFStaticConditionAnalysis::IsDataKey( int8 value )
    {
        return value == _key_type;
    }

    bool KFStaticConditionAnalysis::IsOperatorType( int8 value )
    {
        return _operator_type.find( value ) != _operator_type.end();
    }

    uint32 KFStaticConditionAnalysis::GetOperatorType( int8 value )
    {
        auto iter = _operator_type.find( value );
        if ( iter == _operator_type.end() )
        {
            __LOG_ERROR__( "operator value=[{}] error", value );
            return KFEnum::Null;
        }

        return iter->second;
    }

    uint32 KFStaticConditionAnalysis::GetCheckType( const std::string& value )
    {
        auto iter = _check_type.find( value );
        if ( iter == _check_type.end() )
        {
            return KFEnum::Null;
        }

        return iter->second;
    }

    bool KFStaticConditionAnalysis::IsDelimiter( int8 value )
    {
        for ( auto& iter : _delimiter )
        {
            if ( value == iter.first || value == iter.second )
            {
                return true;
            }
        }

        return false;
    }

    int8 KFStaticConditionAnalysis::GetRightDelimiter( int8 value )
    {
        auto iter = _delimiter.find( value );
        if ( iter == _delimiter.end() )
        {
            return 0;
        }

        return iter->second;
    }

    bool KFStaticConditionAnalysis::IsLinkType( int8 value )
    {
        return _link_type.find( value ) != _link_type.end();
    }

    uint32 KFStaticConditionAnalysis::GetLinkType( int8 value )
    {
        auto iter = _link_type.find( value );
        if ( iter == _link_type.end() )
        {
            return KFEnum::Null;
        }

        return iter->second;
    }

    // level>=1
    // level>=1&money>100
    // level>100|item:1>=2
    // level>money+1&money>100|item:1>2
    StaticConditionsPtr KFStaticConditionAnalysis::Parse( std::string& condition )
    {
        // Cleanup( condition );
        StaticConditionsPtr static_conditions( new KFStaticConditions() );
        if ( !condition.empty() )
        {
            auto start_pos = 0u;
            auto end_pos = 0u;
            auto data = condition.data();
            auto size = ( uint32 )condition.length();

            do
            {
                // 解析条件
                auto ok = false;
                uint32 link_type = 0u;
                std::shared_ptr<KFStaticCondition> static_condition = nullptr;
                std::tie( static_condition, link_type, ok ) = ParseCondition( data, size, start_pos, end_pos );
                static_conditions->_condition_list.push_back( static_condition );
                if ( ok )
                {
                    start_pos = end_pos + 2;
                    if ( link_type != 0u )
                    {
                        static_conditions->_link_type.push_back( link_type );
                    }
                }
            } while ( start_pos < size );
        }

        return static_conditions;
    }

    uint32 KFStaticConditionAnalysis::ReadCondition( const int8* data, uint32 size, uint32 start_pos, uint32& end_pos )
    {
        uint32 link_type = KFEnum::Null;
        for ( auto i = start_pos; i < size; ++i )
        {
            end_pos = i;

            auto value = data[ i ];
            link_type = GetLinkType( value );
            if ( link_type != KFEnum::Null )
            {
                --end_pos;
                break;
            }
        }

        return link_type;
    }

    std::tuple<std::shared_ptr<KFStaticCondition>, uint32, bool> KFStaticConditionAnalysis::ParseCondition( const int8* data, uint32 size, uint32 start_pos, uint32& end_pos )
    {
        auto static_condition = __MAKE_SHARED__( KFStaticCondition );
        auto link_type = ReadCondition( data, size, start_pos, end_pos );

        // 计算因子的长度
        size = end_pos - start_pos + 1;

        // 读取判断类型
        auto check_end_pos = 0u;
        auto check_size = 0u;
        std::tie( static_condition->_check_type, check_end_pos, check_size ) = ReadCheckType( data, size, start_pos );
        if ( static_condition->_check_type == KFEnum::Null )
        {
            return std::make_tuple( static_condition, link_type, false );
        }

        // 读取左边的表达式
        auto ok = false;
        std::tie( static_condition->_left, ok ) = ParseExpression( data, check_end_pos - start_pos + 1, start_pos );
        if ( !ok )
        {
            return std::make_tuple( static_condition, link_type, false );
        }

        start_pos = check_end_pos + check_size + 1;
        std::tie( static_condition->_right, ok ) = ParseExpression( data, end_pos - start_pos + 1, start_pos );
        if ( !ok )
        {
            return std::make_tuple( static_condition, link_type, false );
        }

        return std::make_tuple( static_condition, link_type, true );
    }

    std::tuple<uint32, uint32, uint32> KFStaticConditionAnalysis::ReadCheckType( const int8* data, uint32 size, uint32 start_pos )
    {
        auto end_pos = start_pos + size;
        uint32 check_type = KFEnum::Null;
        auto check_size = 0u;
        for ( auto i = start_pos; i < end_pos; ++i )
        {
            auto ch = data[ i ];
            if ( IsInteger( ch ) || IsLetter( ch ) )
            {
                continue;
            }

            std::string value = "";
            value.push_back( data[ i ] );

            auto first_type = GetCheckType( value );
            if ( first_type != KFEnum::Null )
            {
                if ( i + 1 >= end_pos )
                {
                    break;
                }

                end_pos = i - 1u;
                check_type = first_type;
                check_size = ( uint32 )value.length();

                auto ch = data[ i + 1 ];
                if ( !IsInteger( ch ) && !IsLetter( ch ) )
                {
                    value.push_back( data[i + 1] );
                    auto second_type = GetCheckType( value );
                    if ( second_type != KFEnum::Null )
                    {
                        check_type = second_type;
                        check_size = ( uint32 )value.length();
                    }
                }

                break;
            }
        }

        return std::make_tuple( check_type, end_pos, check_size );
    }

    //level>money+1
    std::tuple<std::shared_ptr<KFStaticConditionExpression>, bool> KFStaticConditionAnalysis::ParseExpression( const int8* data, uint32 size, uint32 start_pos )
    {
        auto static_expression = __MAKE_SHARED__( KFStaticConditionExpression );

        auto end_pos = start_pos + size;
        while ( start_pos < end_pos )
        {
            auto value = data[ start_pos ];
            if ( IsLetter( value ) )
            {
                auto variable = ParseVariable( data, size, start_pos );
                static_expression->_data_list.push_back( variable );
            }
            else if ( IsInteger( value ) )
            {
                auto constant = ParseConstant( data, size, start_pos );
                static_expression->_data_list.push_back( constant );
            }
            else if ( IsOperatorType( value ) )
            {
                --size;
                ++start_pos;
                auto operator_type = GetOperatorType( value );
                static_expression->_operator_list.push_back( operator_type );
            }
            else
            {
                return std::make_tuple( static_expression, false );
            }
        }

        return std::make_tuple( static_expression, true );
    }


    uint32 KFStaticConditionAnalysis::ReadInteger( const int8* data, uint32& size, uint32& start_pos )
    {
        auto count = 0u;
        auto end_pos = start_pos + size;
        for ( auto i = start_pos; i < end_pos; ++i )
        {
            auto value = data[ i ];
            if ( IsInteger( value ) )
            {
                ++count;
            }
            else
            {
                break;
            }
        }

        std::string value = "";
        value.assign( data + start_pos, count );

        size -= count;
        start_pos += count;
        return __TO_UINT32__( value );
    }

    std::string KFStaticConditionAnalysis::ReadLetter( const int8* data, uint32& size, uint32& start_pos )
    {
        auto count = 0u;
        auto end_pos = start_pos + size;
        for ( auto i = start_pos; i < end_pos; ++i )
        {
            auto value = data[ i ];
            if ( IsLetter( value ) )
            {
                ++count;
            }
            else
            {
                break;
            }
        }

        std::string value = "";
        value.assign( data + start_pos, count );

        size -= count;
        start_pos += count;

        return value;
    }

    std::shared_ptr<KFStaticConditionAbstract> KFStaticConditionAnalysis::ParseConstant( const int8* data, uint32& size, uint32& start_pos )
    {
        auto constant = __MAKE_SHARED__( KFStaticConditionConstant );
        constant->_value = ReadInteger( data, size, start_pos );
        return constant;
    }

    std::shared_ptr<KFStaticConditionAbstract> KFStaticConditionAnalysis::ParseVariable( const int8* data, uint32& size, uint32& start_pos )
    {
        auto variable = __MAKE_SHARED__( KFStaticConditionVariable );

        auto end_pos = start_pos + size;
        variable->_data_name = ReadLetter( data, size, start_pos );

        if ( start_pos < end_pos && IsDataKey( data[ start_pos ] ) )
        {
            --size;
            ++start_pos;

            auto value = data[ start_pos ];
            if ( IsInteger( value ) )
            {
                variable->_data_id = ReadInteger( data, size, start_pos );
            }
            else if ( IsLetter( value ) )
            {
                variable->_parent_name = variable->_data_name;
                variable->_data_name = ReadLetter( data, size, start_pos );
            }
        }

        return variable;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

