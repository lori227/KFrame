#ifndef __KF_MEMORY_MANAGE_H__
#define __KF_MEMORY_MANAGE_H__

#include "KFInclude.h"
#include "KFLogMemory.h"
#include "KFBlockStore.h"
#include "KFLogger/KFLogger.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
#define Memory_Alignment 4
#define __USE_MEMORY_POOL__
//////////////////////////////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	template< class Mutex > class KFLogMemory;

	template< class Mutex >
	class KFMemory
	{
	public:
		KFMemory()
		{
			_kf_log_memory = new KFLogMemory< Mutex >();

			_kf_object_mutex = new Mutex();
			_kf_object_block_mutex = new Mutex();

			_kf_memory_mutex = new Mutex();
			_kf_memory_block_mutex = new Mutex();
		}

		~KFMemory()
		{
			ClearObject();
			ClearMemory();
			
			delete _kf_log_memory;
			delete _kf_object_mutex;
			delete _kf_object_block_mutex;
			delete _kf_memory_mutex;
			delete _kf_memory_block_mutex;
		}

		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////
		void* Create( const char* name, uint32 size, uint32 batch, const char* function, uint32 line )
		{
			auto blockstore = CreateObjectBlock( name, size, batch );
			auto object = blockstore->MallocBlock();
			if ( object != nullptr )
			{
				KFLocker< Mutex > locker( *_kf_object_block_mutex );
				_kf_object_block.insert( std::make_pair( object, blockstore ) );
			}
			else
			{
				KFLogger::LogMemory( KFLogger::Error, "[%s:%u] [%s] object malloc failed!", function, line, name );
			}

			return object;
		}

		void Delete( void* object, const char* function, uint32 line )
		{
			if ( object == nullptr )
			{
				return;
			}

			auto blockstore = FindObjectBlock( object );
			if ( blockstore == nullptr )
			{
				return KFLogger::LogMemory( KFLogger::Error, "[%s:%u] object[0x%x] free failed!", function, line, object );
			}

			blockstore->FreeBlock( object );
		}

		////////////////////////////////////////////////////////////////////////////////
		void* Malloc( uint32 size, uint32 batch, const char* function, uint32 line )
		{
			auto blockstore = CreateMemoryBlock( "char", size, batch );
			auto memory = blockstore->MallocBlock();
			if ( memory != nullptr )
			{
				KFLocker< Mutex > locker( *_kf_memory_block_mutex );
				_kf_memory_block.insert( std::make_pair( memory, blockstore ) );
			}
			else
			{
				KFLogger::LogMemory( KFLogger::Error, "[%s:%u] [char] memory malloc failed!", function, line );
			}

			return memory;
		}

		void Free( void* memory, const char* function, uint32 line )
		{
			if ( memory == nullptr )
			{
				return;
			}

			auto blockstore = FindMemoryBlock( memory );
			if ( blockstore == nullptr )
			{
				return KFLogger::LogMemory( KFLogger::Error, "[%s:%u] memory free failed!", function, line );
			}

			blockstore->FreeBlock( memory );
		}
		////////////////////////////////////////////////////////////////////////////////
		void SetLogOpen( bool open )
		{
			_kf_log_memory->SetOpen( open );
		}

	protected:
		// 查找内存分配器
		BlockStore< Mutex >* FindObjectBlock( void* object )
		{
			KFLocker< Mutex > locker( *_kf_object_block_mutex );
			auto iter = _kf_object_block.find( object );
			if ( iter == _kf_object_block.end() )
			{
				return nullptr;
			}

			auto blockstore = iter->second;
			_kf_object_block.erase( iter );
			return blockstore;
		}
		
		BlockStore< Mutex >* CreateObjectBlock( const char* name, uint32 size, uint32 batch )
		{
			KFLocker< Mutex > locker( *_kf_object_mutex );
			auto iter = _kf_object.find( name );
			if ( iter == _kf_object.end() )
			{
				auto blockstore = new BlockStore< Mutex >( name, size, batch, Memory_Alignment );
				iter = _kf_object.insert( std::make_pair( name, blockstore ) ).first;
			}

			return iter->second;
		}

		void ClearObject()
		{
			{
				KFLocker< Mutex > locker( *_kf_object_mutex );
				for ( auto iter : _kf_object )
				{
					delete iter.second;
				}
				_kf_object.clear();
			}

			{
				KFLocker< Mutex > locker( *_kf_object_block_mutex );
				_kf_object_block.clear();
			}
		}

		BlockStore< Mutex >* FindMemoryBlock( void* memory )
		{
			KFLocker< Mutex > locker( *_kf_memory_block_mutex );
			auto iter = _kf_memory_block.find( memory );
			if ( iter == _kf_memory_block.end() )
			{
				return nullptr;
			}

			auto blockstore = iter->second;
			_kf_memory_block.erase( iter );
			return blockstore;
		}
		
		BlockStore< Mutex >* CreateMemoryBlock( const char* name, uint32 size, uint32 batch )
		{
			KFLocker< Mutex > locker( *_kf_memory_mutex );
			auto iter = _kf_memory.find( size );
			if ( iter == _kf_memory.end() )
			{
				auto blockstore = new BlockStore< Mutex >( name, size, batch, Memory_Alignment );
				iter = _kf_memory.insert( std::make_pair( size, blockstore ) ).first;
			}

			return iter->second;
		}

		void ClearMemory()
		{
			{
				KFLocker< Mutex > locker( *_kf_memory_mutex );
				for ( auto iter : _kf_memory )
				{
					delete iter.second;
				}
				_kf_memory.clear();
			}

			{
				KFLocker< Mutex > locker( *_kf_memory_block_mutex );
				_kf_memory_block.clear();
			}
		}
		////////////////////////////////////////////////////////////////////////////
	public:
		// 内存日志
		KFLogMemory< Mutex >* _kf_log_memory;

	protected:
		// 对象分配器
		Mutex * _kf_object_mutex;
		std::map< std::string, BlockStore< Mutex >* > _kf_object;

		// 内存对应的block列表
		Mutex* _kf_object_block_mutex;
		std::map< void*, BlockStore< Mutex >* > _kf_object_block;

		// 内存池
		// 多线程的内存分配器
		Mutex* _kf_memory_mutex;
		std::map< uint32, BlockStore< Mutex >* > _kf_memory;

		// 内存对应的block列表
		Mutex* _kf_memory_block_mutex;
		std::map< void*, BlockStore< Mutex >* > _kf_memory_block;
	};
}

#endif