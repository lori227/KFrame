#ifndef __KF_WORKER_PLUGIN_H__
#define __KF_WORKER_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
	class KFWorkerPlugin : public KFPlugin
	{
	public:
		// 注册
		virtual void Install();

		// 卸载
		virtual void UnInstall();

		// 记载
		virtual void LoadModule();

	};
}


#endif