#include "KFTestPlugin.h"
#include "KFTestModule.h"
#include "KFClient/KFClientInterface.h"
#include "KFServer/KFTcpServerInterface.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFTimer/KFTimerInterface.h"
#include "KFNetFunction/KFNetFunctionInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFTestPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFTestPlugin, KFTestInterface >( new KFTestModule() );
	}

	void KFTestPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFTestPlugin, KFTestInterface >();
	}
	
	void KFTestPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_client = _kf_plugin_manage->FindModule< KFClientInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_net_function = _kf_plugin_manage->FindModule< KFNetFunctionInterface >();
	}
}