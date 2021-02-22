#include "KFHashLogic.hpp"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    void KFHashLogic::AddHashNode( const std::string& name, uint64 id, uint32 virtual_count /* = 1 */ )
    {
        auto node = _node_list.Create( id );

        node->_id = id;
        node->_name = name;

        if ( node->_virtual_list.empty() )
        {
            AddVirtualNode( node, virtual_count );
        }

        _select_list.clear();
    }

    void KFHashLogic::AddVirtualNode( std::shared_ptr<HashNode>& node, uint32 virtual_count )
    {
        for ( auto i = 0u; i < virtual_count; ++i )
        {
            auto temp = __FORMAT__( "{}:{}:{}", node->_name, node->_id, i );

            auto hash_key = KFUtility::GetHashValue( temp );
            node->_virtual_list.push_back( hash_key );

            auto virtual_node = _virtual_list.Create( hash_key );
            virtual_node->_hash_key = hash_key;
            virtual_node->_hash_node = node.get();
        }
    }

    bool KFHashLogic::RemoveHashNode( uint64 id )
    {
        auto node = _node_list.Find( id );
        if ( node == nullptr )
        {
            return false;
        }

        for ( auto hash_key : node->_virtual_list )
        {
            _virtual_list.Remove( hash_key );
        }

        _node_list.Remove( id );
        _select_list.clear();
        return true;
    }

    uint64 KFHashLogic::FindHashNode( const std::string& data, bool cache )
    {
        // 现在已经选择的列表中查找, 优化一下 不要每次都算hash和查找
        if ( cache )
        {
            auto node_id = FindSelectNode( data );
            if ( node_id != _invalid_int )
            {
                return node_id;
            }
        }

        if ( _virtual_list._objects.empty() )
        {
            return _invalid_int;
        }

        auto hash_key = KFUtility::GetHashValue( data );

        // 查找
        auto iter = _virtual_list._objects.lower_bound( hash_key );
        if ( iter == _virtual_list._objects.end() )
        {
            iter = _virtual_list._objects.begin();
        }

        auto virtual_node = iter->second;
        auto node_id = virtual_node->_hash_node->_id;
        if ( cache )
        {
            _select_list[ data ] = node_id;
        }

        return node_id;
    }

    uint64 KFHashLogic::FindHashNode( uint64 object_id, bool cache )
    {
        auto hash_data = __FORMAT__( "object:{}", object_id );
        return FindHashNode( hash_data, cache );
    }

    uint64 KFHashLogic::FindHashNode( const std::string& data, uint64 object_id, bool cache )
    {
        auto hash_data = __FORMAT__( "{}:{}", data, object_id );
        return FindHashNode( hash_data, cache );
    }

    void KFHashLogic::GetAllHashNode( UInt64List& nodes )
    {
        nodes.clear();
        for ( auto iter : _node_list._objects )
        {
            nodes.push_back( iter.first );
        }
    }

    bool KFHashLogic::HaveHashNode( uint64 id )
    {
        return _node_list.Find( id ) != nullptr;
    }

    uint64 KFHashLogic::FindSelectNode( const std::string& data )
    {
        auto iter = _select_list.find( data );
        if ( iter == _select_list.end() )
        {
            return _invalid_int;
        }

        return iter->second;
    }

    uint32 KFHashLogic::GetHashNodeCount()
    {
        return _node_list.Size();
    }

    void KFHashLogic::ClearHashNode()
    {
        _select_list.clear();
        _virtual_list.Clear();
        _node_list.Clear();
    }
}