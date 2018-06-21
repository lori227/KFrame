#include "KFConHash.h"
#include "KFHashLogic.hpp"

//https://www.cnblogs.com/coser/archive/2011/11/27/2265134.html
namespace KFrame
{
	KFConHash::KFConHash()
	{
		_hash_logic = new KFHashLogic();
	}

	KFConHash::~KFConHash()
	{
		delete _hash_logic;
		_hash_logic = nullptr;
	}

	void KFConHash::AddHashNode( const std::string& name, uint32 id, uint32 virtualcount )
	{
		_hash_logic->AddHashNode( name, id, virtualcount );
	}

	bool KFConHash::RemoveHashNode( uint32 id )
	{
		return _hash_logic->RemoveHashNode( id );
	}

	uint32 KFConHash::FindHashNode( uint64 objectid, bool cache /* = false */ )
	{
		return _hash_logic->FindHashNode( objectid, cache );
	}

	uint32 KFConHash::FindHashNode( const std::string& data, bool cache /* = false */ )
	{
		return _hash_logic->FindHashNode( data, cache );
	}

	uint32 KFConHash::FindHashNode( const std::string& data, uint64 objectid, bool cache /* = false */ )
	{
		return _hash_logic->FindHashNode( data, objectid, cache );
	}

	void KFConHash::GetAllHashNode( std::list<uint32>& nodes )
	{
		_hash_logic->GetAllHashNode( nodes );
	}

	bool KFConHash::HaveHashNode( uint32 id )
	{
		return _hash_logic->HaveHashNode( id );
	}

	uint32 KFConHash::GetHashNodeCount()
	{
		return _hash_logic->GetHashNodeCount();
	}
}