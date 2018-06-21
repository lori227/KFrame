#ifndef __KF_MT_RAND_MODULE_H__
#define __KF_MT_RAND_MODULE_H__

#include "KFRandModule.hpp"
#include "KFSingleton.h"

namespace KFrame
{
	class KFMutex;
	class KFMTRandModule : public KFRandModule, public KFSingleton< KFMTRandModule >
	{
	public:
		KFMTRandModule();
		~KFMTRandModule();

		// 随机整数32
		virtual uint32 Rand32();

	protected:
		KFMutex* _kf_mutex;
	};
}



#endif