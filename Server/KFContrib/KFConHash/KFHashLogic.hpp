#ifndef __KF_HASH_LOGIC_H__
#define __KF_HASH_LOGIC_H__

#include "KFInclude.h"
#include "KFMap.h"
#include "KFHashData.hpp"
#include "KFHashFunction.hpp"

namespace KFrame
{
    class KFHashLogic
    {
    public:
        KFHashLogic();
        ~KFHashLogic();

        // 添加Hash节点
        void AddHashNode( const std::string& name, uint64 id, uint32 virtualcount = 100 );

        // 删除Hash节点
        bool RemoveHashNode( uint64 id );

        // 查抄Hash节点
        uint64 FindHashNode( uint64 objectid, bool cache );
        uint64 FindHashNode( const std::string& data, bool cache );
        uint64 FindHashNode( const std::string& data, uint64 objectid, bool cache );

        // 获得所有节点
        void GetAllHashNode( std::list< uint64 >& nodes );

        // 是否存在节点
        bool HaveHashNode( uint64 id );

        // 获得节点数量
        uint32 GetHashNodeCount();

        // 清空节点
        void ClearHashNode();

    protected:
        // 添加虚拟节点
        void AddVirtualNode( HashNode* node, uint32 virtualcount );

        // 查找已经选择的节点
        uint64 FindSelectNode( const std::string& data );
    private:
        // Hash 生产函数
        MD5HashFunction _hash_function;

        // 节点列表
        KFMap< uint64, uint64, HashNode > _node_list;

        // 虚拟节点列表
        KFMap< uint64, uint64, VirtualHashNode > _virtual_list;

        // 已经选择的节点
        std::map< std::string, uint64 > _select_list;
    };
}

#endif
