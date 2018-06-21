#include "KFClassSetting.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
	KFClassSetting::KFClassSetting()
	{

	}

	KFClassSetting::~KFClassSetting()
	{

	}

	void KFClassSetting::AddDataSetting( KFDataSetting& setting )
	{
		_static_data[ setting._name ] = setting;
	}

	const KFDataSetting* KFClassSetting::GetDataSetting( const std::string& name ) const
	{
		auto iter = _static_data.find( name );
		if ( iter == _static_data.end() )
		{
			return nullptr;
		}

		return &iter->second;
	}

	const KFDataSetting* KFClassSetting::GetDataSetting( uint32 index ) const
	{
		auto iter = _indexs.find( index );
		if ( iter == _indexs.end() )
		{
			return nullptr;
		}

		return GetDataSetting( iter->second );
	}
	
	bool KFClassSetting::IsChildData( const std::string& name ) const
	{
		return GetDataSetting( name ) != nullptr;
	}
}