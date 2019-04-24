#include "KFHashLogic.hpp"
#include "KFUtility/KFUtility.h"

namespace KFrame
{
    void KFHashLogic::AddHashNode( const std::string& name, uint64 id, uint32 virtualcount /* = 1 */ )
    {
        auto node = _node_list.Create( id );

        node->_id = id;
        node->_name = name;

        if ( node->_virtual_list.empty() )
        {
            AddVirtualNode( node, virtualcount );
        }

        _select_list.clear();
    }

    void KFHashLogic::AddVirtualNode( HashNode* node, uint32 virtualcount )
    {
        for ( auto i = 0u; i < virtualcount; ++i )
        {
            auto temp = __FORMAT__( "{}:{}:{}", node->_name, node->_id, i );

            auto hashkey = KFUtility::GetHashValue( temp );
            node->_virtual_list.push_back( hashkey );

            auto virtualnode = _virtual_list.Create( hashkey );
            virtualnode->_hash_key = hashkey;
            virtualnode->_hash_node = node;
        }
    }

    bool KFHashLogic::RemoveHashNode( uint64 id )
    {
        auto node = _node_list.Find( id );
        if ( node == nullptr )
        {
            return false;
        }

        for ( auto hashkey : node->_virtual_list )
        {
            _virtual_list.Remove( hashkey );
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
            auto nodeid = FindSelectNode( data );
            if ( nodeid != _invalid_int )
            {
                return nodeid;
            }
        }

        if ( _virtual_list._objects.empty() )
        {
            return _invalid_int;
        }

        auto hashkey = KFUtility::GetHashValue( data );

        // 查找
        auto iter = _virtual_list._objects.lower_bound( hashkey );
        if ( iter == _virtual_list._objects.end() )
        {
            iter = _virtual_list._objects.begin();
        }

        auto virtualnode = iter->second;
        auto nodeid = virtualnode->_hash_node->_id;
        if ( cache )
        {
            _select_list[ data ] = nodeid;
        }

        return nodeid;
    }

    uint64 KFHashLogic::FindHashNode( uint64 objectid, bool cache )
    {
        auto strdata = __FORMAT__( "object:{}", objectid );
        return FindHashNode( strdata, cache );
    }

    uint64 KFHashLogic::FindHashNode( const std::string& data, uint64 objectid, bool cache )
    {
        auto strdata = __FORMAT__( "{}:{}", data, objectid );
        return FindHashNode( strdata, cache );
    }

    void KFHashLogic::GetAllHashNode( std::list<uint64>& nodes )
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