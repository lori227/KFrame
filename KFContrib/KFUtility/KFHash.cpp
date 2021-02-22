#include "KFHash.h"
#include "KFHashLogic.hpp"

//https://www.cnblogs.com/coser/archive/2011/11/27/2265134.html
namespace KFrame
{
    KFHash::KFHash()
    {
        _hash_logic = __NEW_OBJECT__( KFHashLogic );
    }

    KFHash::~KFHash()
    {
        __DELETE_OBJECT__( _hash_logic );
    }

    void KFHash::AddHashNode( const std::string& name, uint64 id, uint32 virtual_count )
    {
        _hash_logic->AddHashNode( name, id, virtual_count );
    }

    bool KFHash::RemoveHashNode( uint64 id )
    {
        return _hash_logic->RemoveHashNode( id );
    }

    uint64 KFHash::FindHashNode( uint64 object_id, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( object_id, cache );
    }

    uint64 KFHash::FindHashNode( const std::string& data, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( data, cache );
    }

    uint64 KFHash::FindHashNode( const std::string& data, uint64 object_id, bool cache /* = false */ )
    {
        return _hash_logic->FindHashNode( data, object_id, cache );
    }

    void KFHash::GetAllHashNode( UInt64List& nodes )
    {
        _hash_logic->GetAllHashNode( nodes );
    }

    bool KFHash::HaveHashNode( uint64 id )
    {
        return _hash_logic->HaveHashNode( id );
    }

    uint32 KFHash::GetHashNodeCount()
    {
        return _hash_logic->GetHashNodeCount();
    }

    void KFHash::ClearHashNode()
    {
        return _hash_logic->ClearHashNode();
    }
}