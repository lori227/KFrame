#include "KFClothesSetting.h"

namespace KFrame
{
	KFClothesSetting::KFClothesSetting()
	{
		_id = 0;
		_quality = 0;
		_mail_id = 0;
	}

	bool KFClothesSetting::HasModle( uint32 modelid ) const
	{
		return _model_list.find( modelid ) != _model_list.end();
	}
}
