#ifndef __KF_HASH_LOGIC_H__
#define __KF_HASH_LOGIC_H__

#include "KFMap.h"
#include "KFHashData.hpp"

namespace KFrame
{
    class KFHashLogic
    {
    public:
        KFHashLogic() = default;
        ~KFHashLogic() = default;

        // 添加Hash节点
        void AddHashNode( const std::string& name, uint64 id, uint32 virtual_count = 100 );

        // 删除Hash节点
        bool RemoveHashNode( uint64 id );

        // 查抄Hash节点
        uint64 FindHashNode( uint64 object_id, bool cache );
        uint64 FindHashNode( const std::string& data, bool cache );
        uint64 FindHashNode( const std::string& data, uint64 object_id, bool cache );

        // 获得所有节点
        void GetAllHashNode( UInt64List& nodes );

        // 是否存在节点
        bool HaveHashNode( uint64 id );

        // 获得节点数量
        uint32 GetHashNodeCount();

        // 清空节点
        void ClearHashNode();

    protected:
        // 添加虚拟节点
        void AddVirtualNode( std::shared_ptr<HashNode>& node, uint32 virtual_count );

        // 查找已经选择的节点
        uint64 FindSelectNode( const std::string& data );
    private:
        // 节点列表
        KFHashMap<uint64, HashNode> _node_list;

        // 虚拟节点列表
        KFMap<uint64, VirtualHashNode> _virtual_list;

        // 已经选择的节点
        StringUInt64 _select_list;
    };
}

#endif
