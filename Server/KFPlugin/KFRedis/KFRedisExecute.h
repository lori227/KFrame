#ifndef __KF_REDIS_EXECUTE_H__
#define __KF_REDIS_EXECUTE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
#include "KFHiRedis.h"
#include "KFRedisFormat.h"
#include "KFRedisException.h"
#include "KFRedisInterface.h"
#include "KFMemory/KFBuffer.h"

namespace KFrame
{
	class KFRedisExecute : public KFRedisDriver
	{
	public:
		/////////////////////////////////////////////////////////////////////////////////////////////
		KFRedisExecute();
		~KFRedisExecute();

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual bool VoidExecute( const char* format, ... );
		virtual bool VoidExecute( const MapString& fieldvalue, const char* format, ... );
		virtual bool VoidExecute( const VectorString& fieldvalue, const char* format, ... );

		virtual bool UInt32Execute( uint32& value, const char* format, ... );
		virtual bool UInt64Execute( uint64& value, const char* format, ... );
		virtual bool StringExecute( std::string& value, const char* format, ... );
		virtual bool MapExecute( MapString& value, const char* format, ... );
		virtual bool VectorExecute( VectorString& value, const char* format, ... );
		virtual bool MapExecute( LesserMapString& value, const char* format, ... );
		/////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////

		// 添加指令
		virtual void AppendCommand( const char* format, ... );
		virtual void AppendCommand( const MapString& fieldvalue, const char* format, ... );
		virtual void AppendCommand( const VectorString& fieldvalue, const char* format, ... );

		// 执行指令集
		virtual bool PipelineExecute();
		virtual bool PipelineExecute( const ListString& commands );
		virtual bool PipelineExecute( ListString& commands, MapString& value);
		virtual bool PipelineExecute( ListString& commands, VectorString& value );

	public:

		// 初始化
		int32 Initialize( const char* ip, int32 port, const char* password );
		
		// 关闭
		void ShutDown();

		// 切换索引
		void SelectIndex( uint32 index );

	private:
		template< class T >
		bool CommandExecute( T& value, const char* command )
		{
			bool result = false;
			try
			{
				result = _redis.Execute< T >( value, command );
			}
			catch ( KFRedisException& exception )
			{
				result = ExecuteException< T >( value, exception );
			}

			return result;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template< class T >
		bool ExecuteException( T& value, KFRedisException& exception )
		{
			if ( !_redis.IsDisconnected() )
			{
				return RetCode< T >( exception );
			}

			_redis.ReConnect();

			bool result = false;
			try
			{
				result = _redis.Execute< T >( value, exception._command.c_str() );
			}
			catch ( KFRedisException& newexception )
			{
				result = RetCode< T >( newexception );
			}

			return result;
		}

		template< class T >
		inline bool RetCode( KFRedisException& exception )
		{
			KFLogger::LogSql( KFLogger::Error, "[%d] [%s] [%s]",
				exception._flags, exception._command.c_str(), exception._display.c_str() );

			return false;
		}
		
	private:
		// redis
		KFHiRedis _redis;

		// 数据库索引
		uint32 _index;

		// 执行的命令
		ListString _commands;

		// buffer
		int8* _buffer;
		uint32 _length;
	};
}
#endif