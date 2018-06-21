#include "KFInt32.h"

namespace KFrame
{
	KFInt32::KFInt32()
	{
		_data = _invalid_int;
	}

	KFInt32::~KFInt32()
	{
	}

	void KFInt32::Reset()
	{
		_data = _invalid_int;
	}

	bool KFInt32::IsValid()
	{
		return _data != _invalid_int;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	void KFInt32::CopyFrom( KFData* kfother )
	{
		SetValue( kfother->GetValue<int32>() );
	}

	void KFInt32::SaveTo( KFData* kfother )
	{
		kfother->SetValue( GetValue<int32>() );
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////

	std::string KFInt32::ToString()
	{
		return KFUtility::ToString( _data );
	}

	void KFInt32::FromString( const std::string& value )
	{
		_data = KFUtility::ToValue<int32>( value );
	}
}
