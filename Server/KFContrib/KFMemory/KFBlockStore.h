#ifndef __BLOCK_STORE_H__
#define __BLOCK_STORE_H__

#include "KFBlockMemory.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	class BlockMemory;
	template< class Mutex >
	class BlockStore
	{
	public:
		BlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment )
		{
			_kf_mutex = new Mutex();
			_block_memory = new BlockMemory( type, blocksize, blockbatch, blockalignment );
		}

		~BlockStore()
		{
			delete _kf_mutex;
			delete _block_memory;
		}

		// 分配内存块
		void* MallocBlock()
		{
			KFLocker< Mutex > locker( *_kf_mutex );
			return _block_memory->Malloc();
		}

		// 释放内存块
		void FreeBlock( void* pobject )
		{
			KFLocker< Mutex > locker( *_kf_mutex );
			_block_memory->Free( pobject );
		}

		// 所有内存大小
		uint64 TotalSize()
		{
			return _block_memory->_total_block_size;
		}

	protected:
		// 互斥量
		Mutex* _kf_mutex;

		// 内存分配器
		BlockMemory* _block_memory;
	};
}
#endif 