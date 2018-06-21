#ifndef __KF_MYSQL_PLUGIN_H__
#define __KF_MYSQL_PLUGIN_H__
#include "KFrame.h"

namespace KFrame
{
	class KFMySQLPlugin : public KFPlugin
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