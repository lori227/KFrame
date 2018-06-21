#include "KFHttpData.h"
#include "KFHttpClient.h"
#include "KFHttpsClient.h"
#include "KFHttpClientManage.h"
#include "KFThread/KFThread.h"

namespace KFrame
{
	KFHttpData::KFHttpData()
	{
		_manage = nullptr;
		_http = nullptr;
		_function = nullptr;
	}

	KFHttpData::~KFHttpData()
	{
		__KF_DESTROY__( KFHttp, _http );
	}

	void KFHttpData::StartMTHttp( KFHttpClientManage* clientmanage )
	{
		_manage = clientmanage;
		KFThread::CreateThread( this, &KFHttpData::Run, __FUNCTION_LINE__ );
	}

	void KFHttpData::Run()
	{
		_result = _http->RunHttp( _url, _data );
		_manage->AddFinishHttp( this );
	}

	void KFHttpData::Finish()
	{
		if ( _function != nullptr )
		{
			_function( _data, _result );
		}
	}
}