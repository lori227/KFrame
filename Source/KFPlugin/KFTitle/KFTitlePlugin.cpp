#include "KFTitlePlugin.h"
#include "KFTitleModule.h"
#include "KFPlayer/KFPlayerInterface.h"
#include "KFKernel/KFKernelInterface.h"
#include "KFComponent/KFComponentInterface.h"
#include "KFNetFunction/KFNetFunctionInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFTitlePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFTitlePlugin, KFTitleInterface >( new KFTitleModule() );
	}

	void KFTitlePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFTitlePlugin, KFTitleInterface >();
	}
	
	void KFTitlePlugin::LoadModule()
	{

	}
}