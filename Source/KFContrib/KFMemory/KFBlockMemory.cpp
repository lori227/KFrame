#include "KFBlockMemory.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	BlockMemory::BlockMemory( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment )
	{
		_memory_type = type;
		_pnext_block = nullptr;
		_total_block_size = 0;
		_block_batch = blockbatch;
		_block_size = (blocksize + blockalignment - 1) & (~(blockalignment - 1));
	}

	BlockMemory::~BlockMemory()
	{
		// 析构的时候不释放, 由上层释放
		// 这个类比其他静态类先析构
		//for ( auto iter = _block_batches.begin(); iter != _block_batches.end(); ++iter )
		//{
		//	uint8* pblock = *iter;
		//	delete[] pblock;
		//}
		//_block_batches.clear();

		//_pnext_block = nullptr;
	}

	void* BlockMemory::Malloc()
	{
		//如果空余内存没有则新开辟一块内存
		if ( _pnext_block == nullptr )
		{
			// 确定T内存大小,再根据内存对齐得出需要申请的单个T内存块大小
			// 新开一批内存(申请的内存比原有内存多15字节,用于下面的字节对齐)

			uint32 newlength = _block_batch * _block_size + 15;
			uint8* pnewbatch = new uint8[ newlength ];

			_total_block_size += newlength;
			_block_batches.push_back( pnewbatch );

			//16字节对齐
			uint8* paligned = reinterpret_cast<uint8*>(reinterpret_cast<uint64>((pnewbatch + 15)) & (~15));

			//将各个元素可以放置的地址确定
			_pnext_block = paligned;

			for ( uint32 i = 0; i < _block_batch - 1; ++i )
			{
				uint8* lastblock = paligned + i * _block_size;
				uint8* nextblock = paligned + (i + 1) * _block_size;
				*(reinterpret_cast<uint64*>(lastblock)) = reinterpret_cast<uint64>(nextblock);
			}

			uint8* nextblock = paligned + (_block_batch - 1) * _block_size;
			*(reinterpret_cast<uint64*>(nextblock)) = static_cast<uint64>(0);
		}

		uint8* pblock = _pnext_block;
		_pnext_block = reinterpret_cast<uint8*>(*reinterpret_cast<uint64*>(_pnext_block));
		return reinterpret_cast<void*>(pblock);
	}

	void BlockMemory::Free( void* pblock )
	{
		if ( pblock == nullptr || _pnext_block == nullptr )
		{
			return;
		}

		*(reinterpret_cast<uint64*>(pblock)) = reinterpret_cast<uint64>(_pnext_block);
		_pnext_block = reinterpret_cast<uint8*>(pblock);
	}

	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
}