#ifndef __BLOCK_MEMORY_H__
#define __BLOCK_MEMORY_H__

#include "KFDefine.h"

namespace KFrame
{
	class BlockMemory
	{
	public:
		BlockMemory( const char* type, uint32 blocksize, uint32 blockbatch, uint32 blockalignment );
		~BlockMemory();

		// 分配内存
		void* Malloc();

		// 释放内存
		void Free( void* pblock );

	public:
		// 类型
		std::string _memory_type;

		// 指向内存块元素下一个位置的指针
		uint8* _pnext_block;

		// 管理零散的内存块指针
		std::vector< uint8* > _block_batches;

		// 每个单体结构的内存块字节数
		uint32 _block_size;

		// 每批申请的内存块数量
		uint32 _block_batch;

		// 公共分配的字节数
		uint64 _total_block_size;
	};

	/////////////////////////////////////////////////////////////////////////////////////////////
}
#endif 