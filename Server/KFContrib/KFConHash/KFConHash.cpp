#include "KFConHash.h"

//https://www.cnblogs.com/coser/archive/2011/11/27/2265134.html
namespace KFrame
{
	KFConHash::KFConHash()
	{

	}

	KFConHash::~KFConHash()
	{

	}

	void KFConHash::AddHashNode( const std::string& name, uint32 id, uint32 virtualcount /* = 1 */ )
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

	void KFConHash::AddVirtualNode( KFHashNode* node, uint32 virtualcount )
	{
		for ( auto i = 0u; i < virtualcount; ++i )
		{
			char temp[ 128 ] = "";
			sprintf( temp, "%s:%u:%u", node->_name.c_str(), node->_id, i );

			auto hashkey = _hash_function.GetHashValue( temp );
			node->_virtual_list.push_back( hashkey );

			auto virtualnode = _virtual_list.Create( hashkey );
			virtualnode->_hash_key = hashkey;
			virtualnode->_hash_node = node;
		}
	}

	bool KFConHash::RemoveHashNode( uint32 id )
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

	uint32 KFConHash::FindHashNode( const std::string& data, bool cache )
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

		auto hashkey = _hash_function.GetHashValue( data.c_str() );

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

	uint32 KFConHash::FindHashNode( uint64 objectid, bool cache )
	{
		auto strid = __KF_STRING__( objectid );

		char temp[ 128 ] = "";
		sprintf( temp, "object:%s", strid.c_str() );

		return FindHashNode( temp, cache );
	}

	uint32 KFConHash::FindHashNode( const std::string& data, uint64 objectid, bool cache )
	{
		auto strid = __KF_STRING__( objectid );

		char temp[ 128 ] = "";
		sprintf( temp, "%s:%s", data.c_str(), strid.c_str() );
		return FindHashNode( temp, cache );
	}

	void KFConHash::GetAllHashNode( std::list<uint32>& nodes )
	{
		nodes.clear();

		for ( auto iter : _node_list._objects )
		{
			nodes.push_back( iter.first );
		}
	}

	bool KFConHash::HaveHashNode( uint32 id )
	{
		return _node_list.Find( id ) != nullptr;
	}

	uint32 KFConHash::FindSelectNode( const std::string& data )
	{
		auto iter = _select_list.find( data );
		if ( iter == _select_list.end() )
		{
			return _invalid_int;
		}

		return iter->second;
	}

	uint32 KFConHash::GetHashNodeCount()
	{
		return _node_list.Size();
	}
}