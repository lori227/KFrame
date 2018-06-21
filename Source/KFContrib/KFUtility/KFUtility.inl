#ifndef __KF_UTILITY_INL__
#define __KF_UTILITY_INL__

#include "KFEnum.h"
#include "KFUtility.h"

namespace KFrame
{
	template< class T >
	inline T KFUtility::Operate( uint32 operate, T basevalue, T value )
	{
		static T element = 1;

		switch ( operate )
		{
		case KFOperateEnum::Set:
			basevalue = value;
			break;
		case KFOperateEnum::Add:
			basevalue += value;
			break;
		case KFOperateEnum::Dec:
			basevalue -= __MIN__( basevalue, value );
			break;
		case KFOperateEnum::And:
			basevalue = ( basevalue & value );
			break;
		case KFOperateEnum::Or:
			basevalue = ( basevalue | value );
			break;
		case KFOperateEnum::Xor:
			basevalue = basevalue & ( ~value );
			break;
		case KFOperateEnum::ABit:
			basevalue |= ( element << value );
			break;
		case KFOperateEnum::XBit:
			basevalue &= ~( element << value );
			break;
		case KFOperateEnum::Greater:
			if ( basevalue < value )
			{
				basevalue = value;
			}
			break;
		case KFOperateEnum::Less:
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
		case KFOperateEnum::Less:
			if ( basevalue <= operatevalue )
			{
				return true;
			}
			break;
		case KFOperateEnum::Greater:
			if ( basevalue >= operatevalue )
			{
				return true;
			}
			break;
		case KFOperateEnum::Equal:
			if ( basevalue == operatevalue )
			{
				return true;
			}
			break;
		default:
			break;
		}

		return false;
	}

	template< class T >
	inline std::string KFUtility::ToString( T value )
	{
		std::stringstream ss;
		ss << value;
		return ss.str();
	}

	template<>
	inline std::string KFUtility::ToString( std::string value )
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
	inline T KFUtility::SplitValue( std::string& srcstring, std::string split )
	{
		auto data = SplitString( srcstring, split );
		return ToValue< T >( data );
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
		return ( value & bitmask ) != 0;
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