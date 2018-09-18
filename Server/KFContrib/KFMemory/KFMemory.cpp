#include "KFrame.h"
#include "KFMemory.h"
#include "KFBlockStore.h"
#include "KFLogMemory.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
#define Memory_Alignment 4
#define __USE_MEMORY_POOL__
//////////////////////////////////////////////////////////////////////////////////////////////////
namespace KFrame
{
    KFMemory::KFMemory()
    {
        _kf_log_memory = new KFLogMemory();

        _kf_object_mutex = new KFMutex();
        _kf_object_block_mutex = new KFMutex();

        _kf_memory_mutex = new KFMutex();
        _kf_memory_block_mutex = new KFMutex();
    }

    KFMemory::~KFMemory()
    {
        {
            KFLocker locker( *_kf_object_mutex );
            for ( auto iter : _mt_object )
            {
                delete iter.second;
            }
            _mt_object.clear();
            _mt_object_block.clear();

            delete _kf_object_mutex;
            delete _kf_object_block_mutex;
        }

        {
            KFLocker locker( *_kf_memory_mutex );
            for ( auto iter : _mt_memory )
            {
                delete iter.second;
            }
            _mt_memory.clear();
            _mt_memory_block.clear();

            delete _kf_memory_mutex;
            delete _kf_memory_block_mutex;
        }

        delete _kf_log_memory;
        _kf_log_memory = nullptr;
    }

    void* KFMemory::Create( const char* name, uint32 size, uint32 batch, const char* function, uint32 line )
    {
        auto blockstore = CreateObjectBlock( name, size, batch );
        auto object = blockstore->MallocBlock();
        if ( object != nullptr )
        {
            KFLocker locker( *_kf_object_block_mutex );
            _mt_object_block.insert( std::make_pair( object, blockstore ) );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "[{}] object malloc failed!", name );
        }

        return object;
    }

    void KFMemory::Delete( void* object, const char* function, uint32 line )
    {
        if ( object == nullptr )
        {
            return;
        }

        auto blockstore = FindObjectBlock( object );
        if ( blockstore != nullptr )
        {
            blockstore->FreeBlock( object );
        }
        else
        {
            __LOG_ERROR_FUNCTION__( function, line, "object[{:p}] free failed!", ( void* )object );
        }
    }

    BlockStore* KFMemory::CreateObjectBlock( const char* name, uint32 size, uint32 batch )
    {
        KFLocker locker( *_kf_object_mutex );

        auto iter = _mt_object.find( name );
        if ( iter == _mt_object.end() )
        {
            auto blockstore = new BlockStore( name, size, batch, Memory_Alignment );
            blockstore->SetLogMemory( _kf_log_memory );
            iter = _mt_object.insert( std::make_pair( name, blockstore ) ).first;
        }

        return iter->second;
    }

    BlockStore* KFMemory::FindObjectBlock( void* object )
    {
        KFLocker locker( *_kf_object_block_mutex );

        auto iter = _mt_object_block.find( object );
        if ( iter == _mt_object_block.end() )
        {
            return nullptr;
        }

        auto blockstore = iter->second;
        _mt_object_block.erase( iter );
        return blockstore;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void* KFMemory::Malloc( uint32 size, uint32 batch, const char* function, uint32 line )
    {
        auto blockstore = CreateMemoryBlock( "char", size, batch );
        auto memory = blockstore->MallocBlock();
        if ( memory != nullptr )
        {
            KFLocker locker( *_kf_memory_block_mutex );
            _mt_memory_block.insert( std::make_pair( memory, blockstore ) );
        }
        else
        {
            __LOG_ERROR__( function, line, "[char] memory malloc failed!" );
        }

        return memory;
    }

    void KFMemory::Free( void* memory, const char* function, uint32 line )
    {
        if ( memory == nullptr )
        {
            return;
        }

        auto blockstore = FindMemoryBlock( memory );
        if ( blockstore != nullptr )
        {
            blockstore->FreeBlock( memory );
        }
        else
        {
            __LOG_ERROR__( function, line, "[char] memory free failed!" );
        }
    }

    BlockStore* KFMemory::CreateMemoryBlock( const char* name, uint32 size, uint32 batch )
    {
        KFLocker locker( *_kf_memory_mutex );

        auto iter = _mt_memory.find( size );
        if ( iter == _mt_memory.end() )
        {
            auto blockstore = new BlockStore( name, size, batch, Memory_Alignment );
            blockstore->SetLogMemory( _kf_log_memory );
            iter = _mt_memory.insert( std::make_pair( size, blockstore ) ).first;
        }

        return iter->second;
    }

    BlockStore* KFMemory::FindMemoryBlock( void* memory )
    {
        KFLocker locker( *_kf_memory_block_mutex );

        auto iter = _mt_memory_block.find( memory );
        if ( iter == _mt_memory_block.end() )
        {
            return nullptr;
        }

        auto blockstore = iter->second;
        _mt_memory_block.erase( iter );
        return blockstore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFMemory::SetLogOpen( bool open )
    {
        _kf_log_memory->SetOpen( open );
    }

#define MILLION_SIZE 1048576.0f
    void KFMemory::PrintLogMemory()
    {
        if ( !_kf_log_memory->IsOpen() )
        {
            return;
        }

        __LOG_DEBUG__( "****************print memory start*****************" );

        uint64 totalusesize = 0;
        uint64 totalmallocsize = 0;

        //auto& stlogblock = _kf_log_memory->GetSTMemory();
        //for ( auto iter = stlogblock._log_data.begin(); iter != stlogblock._log_data.end(); ++iter )
        //{
        //	auto logdata = iter->second;

        //	totalusesize += logdata->_use_size;
        //	totalmallocsize += logdata->_total_size;
        //	PrintLogMemory( iter->first.c_str(), logdata->_count, logdata->_use_size, logdata->_total_size );
        //}

        LogBlock mtlogblock = _kf_log_memory->GetMTMemory();
        for ( auto iter = mtlogblock._log_data.begin(); iter != mtlogblock._log_data.end(); ++iter )
        {
            auto logdata = iter->second;

            totalusesize += logdata->_use_size;
            totalmallocsize += logdata->_total_size;

            __LOG_DEBUG__( "count[{}] use[{}] total[{}] name[{}]", logdata->_count, logdata->_use_size, logdata->_total_size, iter->first );
        }

        __LOG_DEBUG__( "*********print memory end, use[{}], total[{}]********", totalusesize, totalmallocsize );
    }
}
