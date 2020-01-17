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

    void KFConHash::AddHashNode( const std::string& name, uint64 id, uint32 virtualcount )
    {
        _hash_logic->AddHashNode( name, id, virtualcount );
    }

    bool KFConHash::RemoveHashNode( uint64 id )
    {
        return _hash_logic->RemoveHashNode( id );
    }

    uint64 KFConHash::FindHashNode( uint64 objectid, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( objectid, cache );
    }

    uint64 KFConHash::FindHashNode( const std::string& data, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( data, cache );
    }

    uint64 KFConHash::FindHashNode( const std::string& data, uint64 objectid, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( data, objectid, cache );
    }

    void KFConHash::GetAllHashNode( UInt64List& nodes )
    {
        _hash_logic->GetAllHashNode( nodes );
    }

    bool KFConHash::HaveHashNode( uint64 id )
    {
        return _hash_logic->HaveHashNode( id );
    }

    uint32 KFConHash::GetHashNodeCount()
    {
        return _hash_logic->GetHashNodeCount();
    }

    void KFConHash::ClearHashNode()
    {
        return _hash_logic->ClearHashNode();
    }
}