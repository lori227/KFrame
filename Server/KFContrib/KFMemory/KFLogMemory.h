#ifndef __KF_MEMROY_LOGGER_H__
#define __KF_MEMROY_LOGGER_H__

#include "KFDefine.h"

namespace KFrame
{
	class LogData
	{
	public:
		LogData() : _count( 0 ), _use_size( 0 ), _total_size( 0 )
		{

		}

		void AddData( uint64 size, uint64 totalsize );
		void DecData( uint64 size );

	public:
		uint32 _count;
		uint64 _use_size;
		uint64 _total_size;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	class LogBlock
	{
	public:
		void AddBlock( const std::string& type, uint64 size, uint64 totalsize );
		void DecBlock( const std::string& type, uint64 size );

	protected:
		LogData* FindData( const std::string& type );

	public:
		std::map< std::string, LogData* > _log_data;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	template< class Mutex >
	class KFLogMemory
	{
	public:
		KFLogMemory()
		{
			_open = true;
		}

		~KFLogMemory()
		{
			KFLocker< Mutex > locker( *_kf_mutex );
			for ( auto iter : _log_block._log_data )
			{
				delete iter.second;
			}

			_log_block._log_data.clear();;
		}

		// 日志开关
		void SetOpen( bool open )
		{
			_open = open;
		}

		bool IsOpen() const
		{
			return _open;
		}

		LogBlock GetLogBlock()
		{
			return _log_block;
		}

		const LogBlock& RefLogBlock()
		{
			return _log_block;
		}

		void AddMemory( const std::string& type, uint64 size, uint64 totalsize )
		{
			if ( !_open )
			{
				return;
			}

			KFLocker< Mutex > locker( *_kf_mutex );
			_log_block.AddBlock( type, size, totalsize );
		}

		void DecMemory( const std::string& type, uint64 size )
		{
			if ( !_open )
			{
				return;
			}

			KFLocker< Mutex > locker( *_kf_mutex );
			_log_block.DecBlock( type, size );
		}
		/////////////////////////////////////////////////////////////////////////////////////////
	
	private:
		// 日志开关
		bool _open;

		// 互斥量
		Mutex* _kf_mutex;
		
		// 内存记录
		LogBlock _log_block;
	};
}

#endif
