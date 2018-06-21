#include "KFRand.h"
#include "KFSTRandModule.hpp"
#include "KFMTRandModule.hpp"

namespace KFrame
{
	uint32 KFRand::STRand32()
	{
		return KFSTRandModule::Instance()->Rand32();
	}

	double KFRand::STRandFloat()
	{
		return KFSTRandModule::Instance()->RandFloat();

	}
	
	uint32 KFRand::STRandDistrict( uint32 minnumber, uint32 maxnumber, uint32 district )
	{
		if ( maxnumber <= minnumber )
		{
			return minnumber;
		}

		uint32 index = KFSTRandModule::Instance()->Rand32() % (maxnumber - minnumber + district);
		return minnumber + index;
	}

	int32 KFRand::STRandRange( uint32 number )
	{
		number = STRandDistrict( 0, number, 0 );
		uint32 type = KFSTRandModule::Instance()->Rand32() % 2;

		int32 result = 0;
		if ( type == 1 )
		{
			result = static_cast<int32>(number);
		}
		else
		{
			result = 0 - static_cast<int32>(number);
		}

		return result;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	uint32 KFRand::MTRand32()
	{
		return KFMTRandModule::Instance()->Rand32();
	}

	double KFRand::MTRandFloat()
	{
		return KFMTRandModule::Instance()->RandFloat();
	}

	uint32 KFRand::MTRandDistrict( uint32 minnumber, uint32 maxnumber, uint32 district )
	{
		if ( maxnumber <= minnumber )
		{
			return minnumber;
		}

		uint32 index = KFMTRandModule::Instance()->Rand32() % (maxnumber - minnumber + district);
		return minnumber + index;
	}

	int32 KFRand::MTRandRange( uint32 number )
	{
		number = MTRandDistrict( 0, number, 0 );
		uint32 type = KFMTRandModule::Instance()->Rand32() % 2;

		int32 result = 0;
		if ( type == 1 )
		{
			result = static_cast<int32>(number);
		}
		else
		{
			result = 0 - static_cast<int32>(number);
		}

		return result;
	}
}