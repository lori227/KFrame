#ifndef __KF_REDIS_EXCEPTION_H__
#define __KF_REDIS_EXCEPTION_H__

#include "KFInclude.h"

namespace KFrame
{
	class KFRedisException
	{
	public:
		KFRedisException( const char* command, uint32 flags, const char* display )
		{
			_flags = flags;
			_command = command;
			_display = display;
		}

	public:

		// 标识
		int32 _flags;

		// 描述
		std::string _display;

		// 命令
		std::string _command;
	};
}
#endif