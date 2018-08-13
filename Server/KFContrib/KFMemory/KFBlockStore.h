#ifndef __BLOCK_STORE_H__
#define __BLOCK_STORE_H__

#include "KFDefine.h"

namespace KFrame
{
    class BlockMemory;
    class KFLogMemory;
    /////////////////////////////////////////////////////////////////////////////////////////////
    class BlockStore
    {
    public:
        BlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment );

        virtual ~BlockStore();

        void* MallocBlock();
        void FreeBlock( void* pobject );
        void SetLogMemory( KFLogMemory* logmemory );

    protected:
        KFMutex* _kf_mutex;
        BlockMemory* _block_memory;
        KFLogMemory* _kf_log_memory;
    };

    class STBlockStore : public BlockStore
    {
    public:
        STBlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment );
    };

    class MTBlockStore : public BlockStore
    {
    public:
        MTBlockStore( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment );
    };

}
#endif