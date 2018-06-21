#ifndef __KF_REDIS_INTERFACE_H__
#define __KF_REDIS_INTERFACE_H__

#include "KFrame.h"

namespace KFrame
{
	class KFRedisDriver
	{
	public:

		// 执行
		virtual bool VoidExecute( const char* format, ... ) = 0;
		virtual bool VoidExecute( const MapString& fieldvalue, const char* format, ... ) = 0;
		virtual bool VoidExecute( const VectorString& fieldvalue, const char* format, ... ) = 0;

		virtual bool UInt32Execute( uint32& value, const char* format, ... ) = 0;
		virtual bool UInt64Execute( uint64& value, const char* format, ... ) = 0;
		virtual bool StringExecute( std::string& value, const char* format, ... ) = 0;
		virtual bool MapExecute( MapString& value, const char* format, ... ) = 0;
		virtual bool VectorExecute( VectorString& value, const char* format, ... ) = 0;
		virtual bool MapExecute( LesserMapString& value, const char* format, ... ) = 0;
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// 添加指令
		virtual void AppendCommand( const char* format, ... ) = 0;

		virtual void AppendCommand( const MapString& fieldvalue, const char* format, ... ) = 0;
		virtual void AppendCommand( const VectorString& fieldvalue, const char* format, ... ) = 0;

		// 执行指令集
		virtual bool PipelineExecute() = 0;
		virtual bool PipelineExecute( const ListString& commands ) = 0;
		virtual bool PipelineExecute( ListString& commands, MapString& values ) = 0;
		virtual bool PipelineExecute( ListString& commands, VectorString& values ) = 0;
	};

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	class KFRedisInterface : public KFModule
	{
	public:
		// 创建RedisExecute
		virtual KFRedisDriver* CreateExecute( uint32 id ) = 0;
		virtual KFRedisDriver* CreateExecute( const std::string& field, uint32 logicid = 0 ) = 0;
		virtual KFRedisDriver* CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& passwrod ) = 0;
	};


	///////////////////////////////////////////////////////////////////////////////
	__KF_INTERFACE__( _kf_redis, KFRedisInterface );
	///////////////////////////////////////////////////////////////////////////////

}

#endif