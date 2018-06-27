#include "KFRunable.hpp"
#include "KFLogger/KFLogger.h"
#include "KFMemory/KFMalloc.h"

namespace KFrame
{
	void KFRunable::StartThread( KFThreadFunction& function, const char* file, uint32 line )
	{
		_function = function;
		_file = file;
		_line = line;
		_thread.start( *this );
	}


	void KFRunable::run()
	{
		_function();
		__KF_DESTROY__( KFRunable, this );
	}
}