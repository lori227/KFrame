#ifndef __KF_HASH_DATA_H__
#define __KF_HASH_DATA_H__

#include "KFMemory/KFMemoryObject.h"

namespace KFrame
{
	__ST_CLASS__( KFHashNode )
	{
	public:
		// 节点id
		uint32 _id;

		// 节点名字
		std::string _name;

		// 虚拟节点列表
		std::vector< uint32 > _virtual_list;
	};

	__ST_CLASS__( KFVirtualNode )
	{
	public:
		// hash key
		uint32 _hash_key;

		// 真实节点的指针
		KFHashNode* _hash_node;
	};
}

#endif
