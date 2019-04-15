#include "KFRouteService.hpp"

namespace KFrame
{
    void KFRouteService::AddServer( uint64 serverid )
    {
        _server_hash.AddHashNode( _name, serverid, 100 );
        _server_object_count_list[ serverid ] = _invalid_int;

        _min_balance_count = _invalid_int;
        _min_balance_server_id = serverid;
    }

    void KFRouteService::AddObject( uint64 serverid, uint64 objectid, uint32 objectcount )
    {
        _object_list[ objectid ].insert( serverid );

        auto iter = _server_object_count_list.find( serverid );
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

    void KFRouteService::RemoveServer( uint64 serverid )
    {
        _server_hash.RemoveHashNode( serverid );
        _server_object_count_list.erase( serverid );
        for ( auto iter = _object_list.begin(); iter != _object_list.end(); )
        {
            iter->second.erase( serverid );
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

    void KFRouteService::RemoveObject( uint64 serverid, uint64 objectid, uint32 objectcount )
    {
        {
            auto iter = _object_list.find( objectid );
            if ( iter != _object_list.end() )
            {
                iter->second.erase( serverid );
                if ( iter->second.empty() )
                {
                    _object_list.erase( objectid );
                }
            }
        }

        auto iter = _server_object_count_list.find( serverid );
        if ( iter != _server_object_count_list.end() )
        {
            iter->second = objectcount;
            if ( iter->second < _min_balance_count )
            {
                _min_balance_count = iter->second;
                _min_balance_server_id = serverid;
            }
        }
    }

    uint64 KFRouteService::RandServer( uint64 clientid )
    {
        return _server_hash.FindHashNode( clientid, true );
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