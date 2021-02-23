#ifndef __KF_UTILITY_INL__
#define __KF_UTILITY_INL__

#include "KFEnum.h"
#include "KFUtility.h"
#include "KFJson.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
    template<class T>
    inline T KFUtility::Operate( T base_value, uint32 operate, T value )
    {
        static T element = 1u;

        switch ( operate )
        {
        case KFEnum::Set:
            base_value = value;
            break;
        case KFEnum::Add:
            base_value += value;
            break;
        case KFEnum::Dec:
            base_value -= __MIN__( base_value, value );
            break;
        case KFEnum::And:
            base_value = ( base_value & value );
            break;
        case KFEnum::Or:
            base_value |= ( element << value );
            break;
        case KFEnum::Xor:
            base_value &= ~( element << value );
            break;
        case KFEnum::Mul:
            base_value = ( base_value * value );
            break;
        case KFEnum::Div:
            if ( value != 0u )
            {
                base_value = ( base_value / value );
            }
            break;
        case KFEnum::Greater:
            if ( base_value < value )
            {
                base_value = value;
            }
            break;
        case KFEnum::Less:
            if ( base_value > value )
            {
                base_value = value;
            }
            break;
        default:
            break;
        }

        return base_value;
    }


    template<class T>
    inline bool KFUtility::CheckOperate( T base_value, uint32 operate, T operate_value )
    {
        switch ( operate )
        {
        case KFEnum::Less:
            return ( base_value < operate_value );
        case KFEnum::Greater:
            return ( base_value > operate_value );
        case KFEnum::Equal:
            return ( base_value == operate_value );
        case KFEnum::LessEqual:
            return ( base_value <= operate_value );
        case KFEnum::GreaterEqual:
            return ( base_value >= operate_value );
        case KFEnum::UnEqual:
            return ( base_value != operate_value );
        default:
            break;
        }

        return false;
    }

    template<class T>
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

    template<class T>
    inline T KFUtility::ToValue( const std::string& source )
    {
        return ToValue<T>( source.c_str() );
    }

    template<class T>
    inline T KFUtility::ToValue( const char* source )
    {
        return static_cast<T>( atoll( source ) );
    }

    template<>
    inline std::string KFUtility::ToValue( const std::string& source )
    {
        return source;
    }

    template<>
    inline std::string KFUtility::ToValue( const char* source )
    {
        return source;
    }

    template<>
    inline double KFUtility::ToValue( const char* source )
    {
        return static_cast<double>( atof( source ) );
    }

    template<class T>
    inline T KFUtility::SplitValue( std::string& source, const std::string& split )
    {
        auto data = SplitString( source, split );
        return ToValue<T>( data );
    }

    template<class T>
    inline void KFUtility::SplitList( std::string source, const std::string& split, T& out_list )
    {
        out_list.clear();
        while ( !source.empty() )
        {
            auto value = SplitValue<uint32>( source, split );
            out_list.push_back( value );
        }
    }

    template< >
    inline void KFUtility::SplitList( std::string source, const std::string& split, StringList& out_list )
    {
        out_list.clear();
        while ( !source.empty() )
        {
            auto value = SplitString( source, split );
            out_list.push_back( value );
        }
    }

    template< >
    inline void KFUtility::SplitList( std::string source, const std::string& split, StringVector& out_list )
    {
        out_list.clear();
        while ( !source.empty() )
        {
            auto value = SplitString( source, split );
            out_list.push_back( value );
        }
    }

    template<class T>
    inline void KFUtility::SplitSet( std::string source, const std::string& split, T& out_set )
    {
        out_set.clear();
        while ( !source.empty() )
        {
            auto value = SplitValue<uint32>( source, split );
            out_set.insert( value );
        }
    }

    template<>
    inline void KFUtility::SplitSet( std::string source, const std::string& split, StringSet& out_set )
    {
        out_set.clear();
        while ( !source.empty() )
        {
            auto value = SplitString( source, split );
            out_set.insert( value );
        }
    }

    template<class T>
    inline bool KFUtility::ParseArrayList( const std::string& source, T& out_list )
    {
        out_list.clear();
        if ( source.empty() )
        {
            return true;
        }

        __JSON_PARSE_STRING__( kfjson, source );
        if ( !kfjson.IsArray() )
        {
            return false;
        }

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( auto i = 0u; i < size; ++i )
        {
            out_list.push_back( kfjson[i].GetUint() );
        }

        return true;
    }

    template<class T>
    inline bool KFUtility::ParseArraySet( const std::string& source, T& out_set )
    {
        out_set.clear();
        if ( source.empty() )
        {
            return true;
        }

        __JSON_PARSE_STRING__( kfjson, source );
        if ( !kfjson.IsArray() )
        {
            return false;
        }

        auto size = __JSON_ARRAY_SIZE__( kfjson );
        for ( auto i = 0u; i < size; ++i )
        {
            out_set.insert( kfjson[i].GetUint() );
        }

        return true;
    }

    ///////////////////////////////////////////////////////////////////////
    template<class T>
    inline bool KFUtility::HaveBitMask( T value, T bit_mask )
    {
        return ( value & bit_mask ) != 0u;
    }

    template<class T>
    inline void KFUtility::ClearBitMask( T& value, T bit_mask )
    {
        value &= ~bit_mask;
    }

    template<class T>
    inline void KFUtility::AddBitMask( T& value, T bit_mask )
    {
        value |= bit_mask;
    }

}


#endif