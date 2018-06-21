#ifndef __KF_KERNEL_PLUGIN_H__
#define __KF_KERNEL_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
	class KFKernelPlugin : public KFPlugin
	{
	public:
		// 注册
		virtual void Install();

		// 卸载
		virtual void UnInstall();

		// 初始化模块
		virtual void LoadModule();
	};
}

#endif