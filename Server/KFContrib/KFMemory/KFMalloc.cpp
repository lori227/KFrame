#include "KFMalloc.h"
#include "KFLogMemory.h"
#include "KFBuffer.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	KFMalloc* KFMalloc::_kf_malloc = nullptr;

	KFMalloc::KFMalloc()
	{
		_st_memory = new KFMemory< KFNullMutex >();
		_mt_memory = new KFMemory< KFMutex >();
		_kf_buffer = new KFThreadBuffer();
		_kf_mutex = new KFMutex();
	}

	KFMalloc::~KFMalloc()
	{
		delete _st_memory;
		delete _mt_memory;
		delete _kf_buffer;
		delete _kf_mutex;
	}

	void KFMalloc::Initialize( KFMalloc* kfmalloc )
	{
		if ( kfmalloc == nullptr )
		{
			kfmalloc = new KFMalloc;
		}

		KFMalloc::_kf_malloc = kfmalloc;
	}

	KFMalloc* KFMalloc::Instance()
	{
		return KFMalloc::_kf_malloc;
	}

	// 底层是一个内存链表, 每个空内存的前8个字节保存着指向下一个内存的指针,
	// 所以size 不能小于8
	uint32 KFMalloc::CalcNearest2NSize( uint32 size )
	{
		static const uint32 _min_size = 16u;
		if ( size <= _min_size )
		{
			return _min_size;
		}

		static const uint32 _max_size = 1 << 24;
		if ( size > _max_size )
		{
			return 0;
		}

		//power(2, n)
		if ( ( size & ( size - 1 ) ) == 0 )
		{
			return size;
		}

		auto count = 0u;
		while ( size != 0 )
		{
			++count;
			size = size >> 1;
		}

		return 1 << count;
	}

	void* KFMalloc::MallocMemory( uint32 size, uint32 batch, const char* function, uint32 line )
	{
		if ( size == 0 )
		{
			return nullptr;
		}

		auto mallocsize = CalcNearest2NSize( size );
		if ( mallocsize != 0 )
		{
			return _mt_memory->Malloc( mallocsize, batch, function, line );
		}

		KFLogger::LogMemory( KFLogger::Error, "[%s:%u] memory too large[%u]!", function, line, size );
		auto memory = new char[ size ];

		KFLocker< KFMutex > locker( *_kf_mutex );
		_mt_new_memory.insert( memory );

		return memory;
	}

	void KFMalloc::FreeMemory( void* memory, uint32 size, const char* function, uint32 line )
	{
		if ( memory == nullptr || size == 0 )
		{
			return;
		}

		auto mallocsize = CalcNearest2NSize( size );
		if ( mallocsize != 0 )
		{
			_mt_memory->Free( memory, function, line );
		}
		else
		{
			KFLocker< KFMutex > locker( *_kf_mutex );
			auto iter = _mt_new_memory.find( memory );
			if ( iter != _mt_new_memory.end() )
			{
				_mt_new_memory.erase( iter );
				delete[] reinterpret_cast< char* >( memory );
			}
			else
			{
				KFLogger::LogMemory( KFLogger::Error, "[%s:%u] memory not find[%u]!", function, line, size );
			}
		}
	}
			
	int8* KFMalloc::GetInt8( uint32 length, const char* function, uint32 line )
	{
		return _kf_buffer->GetInt8( length, function, line );
	}

	uint8* KFMalloc::GetUInt8( uint32 length, const char* function, uint32 line )
	{
		return _kf_buffer->GetUInt8( length, function, line );
	}

	void KFMalloc::SetLogMemoryOpen( bool open )
	{
		_st_memory->SetLogOpen( open );
		_mt_memory->SetLogOpen( open );
	}

	void KFMalloc::PrintLogMemory()
	{
		if ( !_st_memory->_kf_log_memory->IsOpen() )
		{
			return;
		}

		KFLogger::LogMemory( KFLogger::Info, " " );
		KFLogger::LogMemory( KFLogger::Info, "***********************************print memory start****************************************************" );

		uint64 totalusesize = 0;
		uint64 totalmallocsize = 0;

		auto& stlogblock = _st_memory->_kf_log_memory->RefLogBlock();
		for ( auto iter = stlogblock._log_data.begin(); iter != stlogblock._log_data.end(); ++iter )
		{
			auto logdata = iter->second;

			totalusesize += logdata->_use_size;
			totalmallocsize += logdata->_total_size;
			PrintLogMemory( iter->first.c_str(), logdata->_count, logdata->_use_size, logdata->_total_size );
		}

		auto mtlogblock = _mt_memory->_kf_log_memory->GetLogBlock();
		for ( auto iter = mtlogblock._log_data.begin(); iter != mtlogblock._log_data.end(); ++iter )
		{
			auto logdata = iter->second;

			totalusesize += logdata->_use_size;
			totalmallocsize += logdata->_total_size;
			PrintLogMemory( iter->first.c_str(), logdata->_count, logdata->_use_size, logdata->_total_size );
		}

		auto strtotalusesize = __KF_STRING__( totalusesize );
		auto totalmmallocsize = __KF_STRING__( totalmallocsize );

		KFLogger::LogMemory( KFLogger::Info, " " );
		KFLogger::LogMemory( KFLogger::Info, "******************print memory end, use[%s], total[%s]*************************",
			strtotalusesize.c_str(), totalmmallocsize.c_str() );
	}

	void KFMalloc::PrintLogMemory( const char* type, uint32 count, uint64 usesize, uint64 totalsize )
	{
		auto strusesize = __KF_STRING__( usesize );
		auto strtotalsize = __KF_STRING__( totalsize );

		KFLogger::LogMemory( KFLogger::Info, "count[%u] use[%s] total[%s] [%s]",
			count, strusesize.c_str(), strtotalsize.c_str(), type );
	}
}
