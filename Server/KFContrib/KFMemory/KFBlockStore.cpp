#include "KFBlockStore.h"
#include "KFBlockMemory.h"
#include "KFLogMemory.h"

namespace KFrame
{
	BlockStore::BlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment )
	{
		_kf_mutex = __KF_NEW__( KFMutex );
		_block_memory = __KF_NEW__( BlockMemory, type, blocksize, blockbatch, blockalignment );
	}

	BlockStore::~BlockStore()
	{
		__KF_DELETE__( _kf_mutex );
		__KF_DELETE__( _block_memory );
	}

	void BlockStore::SetLogMemory( KFLogMemory* logmemory )
	{
		_kf_log_memory = logmemory;
	}

	void* BlockStore::MallocBlock()
	{
		KFLocker locker( *_kf_mutex );

		auto block = _block_memory->Malloc();
		_kf_log_memory->AddMTMemory( _block_memory->_memory_type, _block_memory->_block_size, _block_memory->_total_block_size );
		return block;
	}

	void BlockStore::FreeBlock( void* pobject )
	{
		if ( pobject == nullptr )
		{
			return;
		}

		KFLocker locker( *_kf_mutex );

		_block_memory->Free( pobject );
		_kf_log_memory->DecMTMemory( _block_memory->_memory_type, _block_memory->_block_size );
	}
}