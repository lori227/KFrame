#include "KFUInt64.h"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
	KFUInt64::KFUInt64()
	{
		_data = _invalid_int;
	}

	KFUInt64::~KFUInt64()
	{
	}
	
	void KFUInt64::Reset()
	{
		_data = _invalid_int;
	}

	bool KFUInt64::IsValid()
	{
		return _data != _invalid_int;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	void KFUInt64::CopyFrom( KFData* kfother )
	{
		SetValue( kfother->GetValue<uint64>() );
	}

	void KFUInt64::SaveTo( KFData* kfother )
	{
		kfother->SetValue( GetValue<uint64>() );
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFUInt64::ToString()
	{
		return KFUtility::ToString( _data );
	}

	void KFUInt64::FromString( const std::string& value )
	{
		_data = KFUtility::ToValue<uint64>( value );
	}
}
