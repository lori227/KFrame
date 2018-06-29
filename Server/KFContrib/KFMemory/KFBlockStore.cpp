#include "KFBlockStore.h"
#include "KFBlockMemory.h"
#include "KFLogMemory.h"

namespace KFrame
{
    BlockStore::BlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment )
    {
        _kf_mutex = new KFMutex();
        _block_memory = new BlockMemory( type, blocksize, blockbatch, blockalignment );
    }

    BlockStore::~BlockStore()
    {
        delete _kf_mutex;
        delete _block_memory;
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