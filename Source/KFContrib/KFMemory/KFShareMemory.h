#ifndef __KF_SHARE_MEMORY_H__
#define __KF_SHARE_MEMORY_H__

#include "KFrame.h"

namespace KFrame
{
	class KFShareMemory
	{
	public:
		KFShareMemory();
		~KFShareMemory();

		// 创建共享内存
		char* CreateMemory( const std::string& name, uint32 length );

		// 删除共享内存
		void ReleaseMemory();

	private:
		// 共享内存指针
		void* _share_memory;
	};
}

#endif