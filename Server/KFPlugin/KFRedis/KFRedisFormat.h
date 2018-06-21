#ifndef __KF_REDIS_FORMAT_H__
#define __KF_REDIS_FORMAT_H__

#include "KFDefine.h"

namespace KFrame
{
	class KFRedisFormat
	{
	public:

		std::string ToString();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		void Append( const std::string& param );
		void Append( const VectorString& values );
		void Append( const MapString& values );
		void Append( const std::string& command, const std::string& key );
		void Append( const std::string& command, const std::string& key, const std::string& value );
		void Append( const std::string& command, const std::string& key, const std::string& field, const std::string& value );
		void Append( const std::string& command, const VectorString& values );
		void Append( const std::string& command, const std::string& key, const VectorString& values );
		void Append( const std::string& command, const MapString& values );
		void Append( const std::string& command, const std::string& key, const MapString& values );

	private:
		// 参数列表
		ListString _params;
	};

}
#endif