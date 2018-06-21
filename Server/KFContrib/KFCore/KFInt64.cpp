#include "KFInt64.h"

namespace KFrame
{
	KFInt64::KFInt64()
	{
		_data = _invalid_int;
	}

	KFInt64::~KFInt64()
	{
	}
	
	void KFInt64::Reset()
	{
		_data = _invalid_int;
	}

	bool KFInt64::IsValid()
	{
		return _data != _invalid_int;
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	void KFInt64::CopyFrom( KFData* kfother )
	{
		SetValue( kfother->GetValue<int64>() );
	}

	void KFInt64::SaveTo( KFData* kfother )
	{
		kfother->SetValue( GetValue<int64>() );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFInt64::ToString()
	{
		return KFUtility::ToString( _data );
	}

	void KFInt64::FromString( const std::string& value )
	{
		_data = KFUtility::ToValue<int64>( value );
	}
}
