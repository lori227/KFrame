#ifndef __KF_RUNABLE_H__
#define __KF_RUNABLE_H__

#include "KFDefine.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

namespace KFrame
{
	///////////////////////////////////////////////////////////////////////////////////////////
	__MT_CLASS__( KFRunable ), public Poco::Runnable
	{
	public:
		// 开启线程
		void StartThread( KFThreadFunction& function, const char* file, uint32 line );

		// poco run
		virtual void run();

	private:
		// 线程
		Poco::Thread _thread;

		// 回调函数
		std::string _file;
		uint32 _line;
		KFThreadFunction _function;
	};

	
}

#endif
