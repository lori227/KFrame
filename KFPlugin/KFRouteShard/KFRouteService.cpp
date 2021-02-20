#include "KFRouteService.hpp"

namespace KFrame
{
    void KFRouteService::AddServer( uint64 server_id )
    {
        _server_hash.AddHashNode( _name, server_id, 100 );
        _server_object_count_list[ server_id ] = _invalid_int;

        _min_balance_count = _invalid_int;
        _min_balance_server_id = server_id;
    }

    void KFRouteService::AddObject( uint64 server_id, uint64 objectid, uint32 objectcount )
    {
        _object_list[ objectid ].insert( server_id );

        auto iter = _server_object_count_list.find( server_id );
        if ( iter != _server_object_count_list.end() )
        {
            iter->second = objectcount;
            CalcMinBalance();
        }
    }

    void KFRouteService::CalcMinBalance()
    {
        _min_balance_count = __MAX_UINT32__;
        _min_balance_server_id = _invalid_int;
        for ( auto& iter : _server_object_count_list )
        {
            if ( iter.second < _min_balance_count )
            {
                _min_balance_count = iter.second;
                _min_balance_server_id = iter.first;
            }
        }
    }

    void KFRouteService::RemoveServer( uint64 server_id )
    {
        _server_hash.RemoveHashNode( server_id );
        _server_object_count_list.erase( server_id );
        for ( auto iter = _object_list.begin(); iter != _object_list.end(); )
        {
            iter->second.erase( server_id );
            if ( iter->second.empty() )
            {
                iter = _object_list.erase( iter );
            }
            else
            {
                ++iter;
            }
        }

        CalcMinBalance();
    }

    void KFRouteService::RemoveObject( uint64 server_id, uint64 objectid, uint32 objectcount )
    {
        {
            auto iter = _object_list.find( objectid );
            if ( iter != _object_list.end() )
            {
                iter->second.erase( server_id );
                if ( iter->second.empty() )
                {
                    _object_list.erase( objectid );
                }
            }
        }

        auto iter = _server_object_count_list.find( server_id );
        if ( iter != _server_object_count_list.end() )
        {
            iter->second = objectcount;
            if ( iter->second < _min_balance_count )
            {
                _min_balance_count = iter->second;
                _min_balance_server_id = server_id;
            }
        }
    }

    uint64 KFRouteService::RandServer( uint64 randid )
    {
        return _server_hash.FindHashNode( randid, false );
    }

    uint64 KFRouteService::BalanceServer()
    {
        return _min_balance_server_id;
    }

    uint64 KFRouteService::ObjectServer( uint64 objectid )
    {
        auto iter = _object_list.find( objectid );
        if ( iter == _object_list.end() )
        {
            return _invalid_int;
        }

        auto count = iter->second.size();
        if ( count == 0u )
        {
            return _invalid_int;
        }

        auto index = KFGlobal::Instance()->RandRatio( count );
        auto begin = iter->second.begin();
        std::advance( begin, index );
        return *begin;
    }
}