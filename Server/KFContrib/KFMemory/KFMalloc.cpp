#include "KFMalloc.h"
#include "KFLogMemory.h"
#include "KFBuffer.h"

namespace KFrame
{
	KFMalloc* KFMalloc::_kf_malloc = nullptr;

	KFMalloc::KFMalloc()
	{
		_kf_memory = new KFMemory();
		_kf_buffer = new KFThreadBuffer();

	}

	KFMalloc::~KFMalloc()
	{
		delete _kf_memory;
		delete _kf_buffer;
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
			return _kf_memory->Malloc( mallocsize, batch, function, line );
		}

		KFLogger::LogMemory( KFLogger::Error, "[%s:%u] memory too large[%u]!", function, line, size );
		auto memory = new char[ size ];
		_new_memory.insert( memory );
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
			_kf_memory->Free( memory, function, line );
		}
		else
		{
			auto iter = _new_memory.find( memory );
			if ( iter != _new_memory.end() )
			{
				_new_memory.erase( iter );
				delete[] reinterpret_cast< char* >( memory );
			}
			else
			{
				KFLogger::LogMemory( KFLogger::Error, "[%s:%u] memory not find[%u]!", function, line, size );
			}
		}
	}

	void KFMalloc::SetLogMemoryOpen( bool open )
	{
		_kf_memory->SetLogOpen( open );
	}

	void KFMalloc::PrintLogMemory()
	{
		_kf_memory->PrintLogMemory();
	}


	int8* KFMalloc::GetInt8( uint32 length, const char* function, uint32 line )
	{
		return _kf_buffer->GetInt8( length, function, line );
	}

	uint8* KFMalloc::GetUInt8( uint32 length, const char* function, uint32 line )
	{
		return _kf_buffer->GetUInt8( length, function, line );
	}
}
