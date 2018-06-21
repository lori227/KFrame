#ifndef __KF_HASH_FUNCTION_H__
#define __KF_HASH_FUNCTION_H__

#include "KFInclude.h"

namespace KFrame
{
	class HashFunction
	{
	public:
		virtual uint32 GetHashValue( const char* data ) = 0;
	};
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class MD5HashFunction : public HashFunction
	{
	public:
		virtual uint32 GetHashValue( const char* data );
	};
}

#endif
