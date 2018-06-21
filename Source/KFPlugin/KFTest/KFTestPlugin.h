#ifndef __KF_TEST_PLUGIN_H__
#define __KF_TEST_PLUGIN_H__

#include "KFPlugin/KFPlugin.h"

namespace KFrame
{
	class KFTestPlugin : public KFPlugin
	{
	public:
		// в╒╡А
		virtual void Install();

		// п╤ть
		virtual void UnInstall();

		// ╪сть
		virtual void LoadModule();
	};
}


#endif