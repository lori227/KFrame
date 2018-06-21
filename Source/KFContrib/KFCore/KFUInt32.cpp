#include "KFUInt32.h"

namespace KFrame
{
	KFUInt32::KFUInt32()
	{
		_data = _invalid_int;
	}

	KFUInt32::~KFUInt32()
	{
	}

	void KFUInt32::Reset()
	{
		_data = _invalid_int;
	}

	bool KFUInt32::IsValid()
	{
		return _data != _invalid_int;
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	void KFUInt32::CopyFrom( KFData* kfother )
	{
		SetValue( kfother->GetValue<uint32>() );
	}

	void KFUInt32::SaveTo( KFData* kfother )
	{
		kfother->SetValue( GetValue<uint32>() );
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	std::string KFUInt32::ToString()
	{
		return KFUtility::ToString( _data );
	}

	void KFUInt32::FromString( const std::string& value )
	{
		_data = KFUtility::ToValue<uint32>( value );
	}
}
