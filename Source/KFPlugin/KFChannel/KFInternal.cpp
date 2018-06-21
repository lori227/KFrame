#include "KFInternal.h"
#include "KFJson.h"

namespace KFrame
{
	KFInternal::KFInternal( uint32 channel ) : KFChannel( channel )
	{
		
	}

	KFInternal::~KFInternal()
	{

	}
	
	std::string KFInternal::RequestLogin( KFJson& json, const KFChannelSetting* kfchannelsetting )
	{
		auto account = json.GetString( KFField::_account );

		// 测试服直接登录
		KFJson response;
		response.SetValue( KFField::_account, account );
		response.SetValue( KFField::_channel, _channel );
		response.SetValue( KFField::_app_id, kfchannelsetting->_app_id );
		response.SetValue( KFField::_app_key, kfchannelsetting->_app_key );
		return _kf_http_server->SendResponse( response );
	}

	std::string KFInternal::RequestPay( KFJson& json, const KFChannelSetting* kfchannelsetting )
	{
		return _invalid_str;
	}
}