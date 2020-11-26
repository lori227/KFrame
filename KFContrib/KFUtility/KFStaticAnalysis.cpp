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
    StaticConditionsPtr KFStaticConditionAnalysis::Parse( std::string& strcondition )
    {
        // Cleanup( strcondition );
        StaticConditionsPtr staticconditions( new KFStaticConditions() );
        if ( !strcondition.empty() )
        {
            auto startpos = 0u;
            auto endpos = 0u;
            auto data = strcondition.data();
            auto size = ( uint32 )strcondition.length();

            do
            {
                // 解析条件
                auto ok = false;
                uint32 linktype = 0u;
                KFStaticCondition* kfcondition = nullptr;
                std::tie( kfcondition, linktype, ok ) = ParseCondition( data, size, startpos, endpos );

                staticconditions->_condition_list.push_back( kfcondition );
                if ( ok )
                {
                    startpos = endpos + 2;
                    if ( linktype != 0u )
                    {
                        staticconditions->_link_type.push_back( linktype );
                    }
                }
            } while ( startpos < size );
        }

        return staticconditions;
    }

    uint32 KFStaticConditionAnalysis::ReadCondition( const int8* data, uint32 size, uint32 startpos, uint32& endpos )
    {
        uint32 linktype = KFEnum::Null;
        for ( auto i = startpos; i < size; ++i )
        {
            endpos = i;

            auto value = data[ i ];
            linktype = GetLinkType( value );
            if ( linktype != KFEnum::Null )
            {
                --endpos;
                break;
            }
        }

        return linktype;
    }

    std::tuple<KFStaticCondition*, uint32, bool> KFStaticConditionAnalysis::ParseCondition( const int8* data, uint32 size, uint32 startpos, uint32& endpos )
    {
        auto kfcondition = __KF_NEW__( KFStaticCondition );
        auto linktype = ReadCondition( data, size, startpos, endpos );

        // 计算因子的长度
        size = endpos - startpos + 1;

        // 读取判断类型
        auto checkendpos = 0u;
        auto checksize = 0u;
        std::tie( kfcondition->_check_type, checkendpos, checksize ) = ReadCheckType( data, size, startpos );
        if ( kfcondition->_check_type == KFEnum::Null )
        {
            return std::make_tuple( kfcondition, linktype, false );
        }

        // 读取左边的表达式
        auto ok = false;
        std::tie( kfcondition->_left, ok ) = ParseExpression( data, checkendpos - startpos + 1, startpos );
        if ( !ok )
        {
            return std::make_tuple( kfcondition, linktype, false );
        }

        startpos = checkendpos + checksize + 1;
        std::tie( kfcondition->_right, ok ) = ParseExpression( data, endpos - startpos + 1, startpos );
        if ( !ok )
        {
            return std::make_tuple( kfcondition, linktype, false );
        }

        return std::make_tuple( kfcondition, linktype, true );
    }

    std::tuple<uint32, uint32, uint32> KFStaticConditionAnalysis::ReadCheckType( const int8* data, uint32 size, uint32 startpos )
    {
        auto endpos = startpos + size;
        uint32 checktype = KFEnum::Null;
        auto checksize = 0u;
        for ( auto i = startpos; i < endpos; ++i )
        {
            auto ch = data[ i ];
            if ( IsInteger( ch ) || IsLetter( ch ) )
            {
                continue;
            }

            std::string value = "";
            value.push_back( data[ i ] );

            auto firsttype = GetCheckType( value );
            if ( firsttype != KFEnum::Null )
            {
                if ( i + 1 >= endpos )
                {
                    break;
                }

                endpos = i - 1u;
                checktype = firsttype;
                checksize = ( uint32 )value.length();

                auto ch = data[ i + 1 ];
                if ( !IsInteger( ch ) && !IsLetter( ch ) )
                {
                    value.push_back( data[i + 1] );
                    auto secondtype = GetCheckType( value );
                    if ( secondtype != KFEnum::Null )
                    {
                        checktype = secondtype;
                        checksize = ( uint32 )value.length();
                    }
                }

                break;
            }
        }

        return std::make_tuple( checktype, endpos, checksize );
    }

    //level>money+1
    std::tuple<KFStaticConditionExpression*, bool> KFStaticConditionAnalysis::ParseExpression( const int8* data, uint32 size, uint32 startpos )
    {
        auto kfexpression = __KF_NEW__( KFStaticConditionExpression );

        auto endpos = startpos + size;
        while ( startpos < endpos )
        {
            auto value = data[ startpos ];
            if ( IsLetter( value ) )
            {
                auto kfdata = ParseVariable( data, size, startpos );
                kfexpression->_data_list.push_back( kfdata );
            }
            else if ( IsInteger( value ) )
            {
                auto kfdata = ParseConstant( data, size, startpos );
                kfexpression->_data_list.push_back( kfdata );
            }
            else if ( IsOperatorType( value ) )
            {
                --size;
                ++startpos;
                auto operatortype = GetOperatorType( value );
                kfexpression->_operator_list.push_back( operatortype );
            }
            else
            {
                return std::make_tuple( kfexpression, false );
            }
        }

        return std::make_tuple( kfexpression, true );
    }


    uint32 KFStaticConditionAnalysis::ReadInteger( const int8* data, uint32& size, uint32& startpos )
    {
        auto count = 0u;
        auto endpos = startpos + size;
        for ( auto i = startpos; i < endpos; ++i )
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
        value.assign( data + startpos, count );

        size -= count;
        startpos += count;
        return __TO_UINT32__( value );
    }

    std::string KFStaticConditionAnalysis::ReadLetter( const int8* data, uint32& size, uint32& startpos )
    {
        auto count = 0u;
        auto endpos = startpos + size;
        for ( auto i = startpos; i < endpos; ++i )
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
        value.assign( data + startpos, count );

        size -= count;
        startpos += count;

        return value;
    }

    KFStaticConditionAbstract* KFStaticConditionAnalysis::ParseConstant( const int8* data, uint32& size, uint32& startpos )
    {
        auto kfdata = __KF_NEW__( KFStaticConditionConstant );
        kfdata->_value = ReadInteger( data, size, startpos );
        return kfdata;
    }

    KFStaticConditionAbstract* KFStaticConditionAnalysis::ParseVariable( const int8* data, uint32& size, uint32& startpos )
    {
        auto kfdata = __KF_NEW__( KFStaticConditionVariable );

        auto endpos = startpos + size;
        kfdata->_data_name = ReadLetter( data, size, startpos );

        if ( startpos < endpos &&
                IsDataKey( data[ startpos ] ) )
        {
            --size;
            ++startpos;

            auto value = data[ startpos ];
            if ( IsInteger( value ) )
            {
                kfdata->_data_id = ReadInteger( data, size, startpos );
            }
            else if ( IsLetter( value ) )
            {
                kfdata->_parent_name = kfdata->_data_name;
                kfdata->_data_name = ReadLetter( data, size, startpos );
            }
        }

        return kfdata;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

