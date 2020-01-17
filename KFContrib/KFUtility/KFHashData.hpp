#ifndef __KF_HASH_DATA_H__
#define __KF_HASH_DATA_H__

#include "KFInclude.h"
#include "KFMap.h"

namespace KFrame
{
    class HashNode
    {
    public:
        // 节点id
        uint64 _id = 0;

        // 节点名字
        std::string _name;

        // 虚拟节点列表
        UInt64Vector _virtual_list;
    };

    class VirtualHashNode
    {
    public:
        // hash key
        uint32 _hash_key;

        // 真实节点的指针
        HashNode* _hash_node;
    };
}

#endif
