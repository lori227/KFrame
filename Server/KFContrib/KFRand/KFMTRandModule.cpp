#include "KFMTRandModule.hpp"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	KFMTRandModule::KFMTRandModule()
	{
		_kf_mutex = new KFMutex();
	}

	KFMTRandModule::~KFMTRandModule()
	{
		delete _kf_mutex;
	}

	uint32 KFMTRandModule::Rand32()
	{
		KFLocker< KFMutex > locker( *_kf_mutex );

		return KFRandModule::Rand32();
	}
}