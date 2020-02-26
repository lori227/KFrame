#ifndef __KF_UTILITY_INL__
#define __KF_UTILITY_INL__

#include "KFEnum.h"
#include "KFUtility.h"
#include "KFJson.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
    template< class T >
    inline T KFUtility::Operate( uint32 operate, T basevalue, T value )
    {
        static T element = 1u;

        switch ( operate )
        {
        case KFEnum::Set:
            basevalue = value;
            break;
        case KFEnum::Add:
            basevalue += value;
            break;
        case KFEnum::Dec:
            basevalue -= __MIN__( basevalue, value );
            break;
        case KFEnum::And:
            basevalue = ( basevalue & value );
            break;
        case KFEnum::Or:
            basevalue |= ( element << value );
            break;
        case KFEnum::Xor:
            basevalue &= ~( element << value );
            break;
        case KFEnum::Mul:
            basevalue = ( basevalue * value );
            break;
        case KFEnum::Div:
            if ( value != 0u )
            {
                basevalue = ( basevalue / value );
            }
            break;
        case KFEnum::Greater:
            if ( basevalue < value )
            {
                basevalue = value;
            }
            break;
        case KFEnum::Less:
            if ( basevalue > value )
            {
                basevalue = value;
            }
            break;
        default:
            break;
        }

        return basevalue;
    }


    template< class T >
    inline bool KFUtility::CheckOperate( T basevalue, uint32 operate, T operatevalue )
    {
        switch ( operate )
        {
        case KFEnum::Less:
            return ( basevalue < operatevalue );
        case KFEnum::Greater:
            return ( basevalue > operatevalue );
        case KFEnum::Equal:
            return ( basevalue == operatevalue );
        case KFEnum::LessEqual:
            return ( basevalue <= operatevalue );
        case KFEnum::GreaterEqual:
            return ( basevalue >= operatevalue );
        case KFEnum::UnEqual:
            return ( basevalue != operatevalue );
        default:
            break;
        }

        return false;
    }

    template< class T >
    inline std::string KFUtility::ToString( const T& value )
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<>
    inline std::string KFUtility::ToString( const std::string& value )
    {
        return value;
    }

    template< class T >
    inline T KFUtility::ToValue( const std::string& srcstring )
    {
        return ToValue< T >( srcstring.c_str() );
    }

    template< class T >
    inline T KFUtility::ToValue( const char* srcstring )
    {
        return static_cast< T >( atoll( srcstring ) );
    }

    template<>
    inline std::string KFUtility::ToValue( const std::string& srcstring )
    {
        return srcstring;
    }

    template<>
    inline std::string KFUtility::ToValue( const char* srcstring )
    {
        return srcstring;
    }

    template< class T >
    inline T KFUtility::SplitValue( std::string& srcstring, const std::string& split )
    {
        auto data = SplitString( srcstring, split );
        return ToValue< T >( data );
    }

    template< class T >
    inline void KFUtility::SplitList( T& outlist, std::string& srcstring, const std::string& split )
    {
        outlist.clear();
        while ( !srcstring.empty() )
        {
            auto value = SplitValue< uint32 >( srcstring, split );
            outlist.push_back( value );
        }
    }

    template< class T >
    inline void KFUtility::SplitSet( T& outlist, std::string& srcstring, const std::string& split )
    {
        outlist.clear();
        while ( !srcstring.empty() )
        {
            auto value = SplitValue< uint32 >( srcstring, split );
            outlist.insert( value );
        }
    }

    template< class T >
    inline bool KFUtility::ParseArrayList( T& arraylist, const std::string& srcstring )
    {
        arraylist.clear();
        if ( srcstring.empty() )
        {
            return true;
        }

        __JSON_PARSE_STRING__( kfjson, srcstring );
        if ( !kfjson.IsArray() )
        {
            return false;
        }

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( auto i = 0u; i < size; ++i )
        {
            arraylist.push_back( kfjson[i].GetUint() );
        }

        return true;
    }

    template< class T >
    inline bool KFUtility::ParseArraySet( T& arrayset, const std::string& srcstring )
    {
        arrayset.clear();
        if ( srcstring.empty() )
        {
            return true;
        }

        __JSON_PARSE_STRING__( kfjson, srcstring );
        if ( !kfjson.IsArray() )
        {
            return false;
        }

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( auto i = 0u; i < size; ++i )
        {
            arrayset.insert( kfjson[i].GetUint() );
        }

        return true;
    }


    ///////////////////////////////////////////////////////////////////////
    template<>
    inline double KFUtility::ToValue( const char* srcstring )
    {
        return static_cast< double >( atof( srcstring ) );
    }

    ///////////////////////////////////////////////////////////////////////
    template< class T >
    inline bool KFUtility::HaveBitMask( T value, T bitmask )
    {
        return ( value & bitmask ) != 0u;
    }

    template< class T >
    inline void KFUtility::ClearBitMask( T& value, T bitmask )
    {
        value &= ~bitmask;
    }

    template< class T >
    inline void KFUtility::AddBitMask( T& value, T bitmask )
    {
        value |= bitmask;
    }

}


#endif