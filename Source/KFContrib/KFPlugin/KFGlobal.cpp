#include "KFGlobal.h"

namespace KFrame
{
	KFGlobal* KFGlobal::_kf_global = nullptr;

	KFGlobal::KFGlobal()
	{
		_app_run = true;
		_app_id = 0;
		_game_time = 0;
		_real_time = 0;
		_listen_port = 0;
	}

	KFGlobal::~KFGlobal()
	{
	}

	void KFGlobal::Initialize( KFGlobal* kfglobal )
	{
		if ( kfglobal == nullptr )
		{
			kfglobal = new KFGlobal;
		}

		KFGlobal::_kf_global = kfglobal;
	}

	KFGlobal* KFGlobal::Instance()
	{
		return KFGlobal::_kf_global;
	}

	std::string KFGlobal::FormatTitleText( const std::string& appname, const std::string& apptype, uint32 appid )
	{
		char temp[ 128 ] = { 0 };
		sprintf( temp, "%s-%s-%u", appname.c_str(), apptype.c_str(), appid );
		return temp;
	}

}