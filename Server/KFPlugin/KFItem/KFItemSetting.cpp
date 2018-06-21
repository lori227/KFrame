#include "KFItemSetting.h"

namespace KFrame
{
	KFItemSetting::KFItemSetting()
	{
		_id = 0;
		_type = 0;
		_overlay_type = 0;
		_overlay_count = 0;
		_use_count = 0;
		_time_type = 0;
		_valid_time = 0;
		_quality = 0;
		
	}

	bool KFItemSetting::CheckCanUse() const
	{
		return _use_count > 0;
	}

	bool KFItemSetting::IsRealItem() const
	{
		switch ( _type )
		{
		case KFItemEnum::Script:
			return false;
			break;
		default:
			break;
		}

		return true;
	}

	const std::string& KFItemSetting::GetFunction( uint32 type ) const
	{
		auto iter = _function.find( type );
		if ( iter == _function.end() )
		{
			return _invalid_str;
		}

		return iter->second;
	}
}
