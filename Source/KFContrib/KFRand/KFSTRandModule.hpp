#ifndef __KF_ST_RAND_MODULE_H__
#define __KF_ST_RAND_MODULE_H__

#include "KFRandModule.hpp"
#include "KFSingleton.h"

namespace KFrame
{
	class KFSTRandModule : public KFRandModule, public KFSingleton< KFSTRandModule >
	{
	public:
		KFSTRandModule();
	};
}



#endif