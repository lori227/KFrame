#ifndef __KF_OPTION_INTERFACE_H__
#define __KF_OPTION_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
	class KFOptionInterface : public KFModule
	{
	public:
		// 获得string配置
		virtual const std::string& GetString( const std::string& name, uint32 key = 0 ) = 0;

		// 获得uint32配置
		virtual uint32 GetUInt32( const std::string& name, uint32 key = 0 ) = 0;

	};

	///////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_option, KFOptionInterface );
	///////////////////////////////////////////////////////////////////////////
}



#endif