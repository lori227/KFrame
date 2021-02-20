#include "KFrame.h"
#include "KFUuid.hpp"
#include "KFUuidGenerate.hpp"
#include "KFUuidData.hpp"

namespace KFrame
{
    KFUuid::KFUuid()
    {
        _kf_mutex = __NEW_OBJECT__( KFMutex );
    }

    KFUuid::~KFUuid()
    {
        __DELETE_OBJECT__( _kf_mutex );
        for ( auto& iter : _uuid_data_list )
        {
            delete iter.second;
        }
        _uuid_data_list.clear();

        for ( auto& iter : _uuid_setting_list )
        {
            delete iter.second;
        }
        _uuid_setting_list.clear();
    }

    void KFUuid::AddData( const std::string& name, uint64 project_time, uint32 time_bits, uint32 zone_bits, uint32 worker_bits, uint32 seq_bits )
    {
        auto iter = _uuid_setting_list.find( name );
        if ( iter == _uuid_setting_list.end() )
        {
            auto kfsetting = __NEW_OBJECT__( KFUuidData );
            iter = _uuid_setting_list.insert( std::make_pair( name, kfsetting ) ).first;
        }

        iter->second->InitData( project_time, time_bits, zone_bits, worker_bits, seq_bits );
    }

    const KFUuidData* KFUuid::FindUuidData( const std::string& name )
    {
        auto iter = _uuid_setting_list.find( name );
        if ( iter == _uuid_setting_list.end() )
        {
            iter = _uuid_setting_list.find( __STRING__( normal ) );
        }

        return iter->second;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFUuidGenerate* KFUuid::CreateUuidGenerate( const std::string& name )
    {
        auto iter = _uuid_data_list.find( name );
        if ( iter != _uuid_data_list.end() )
        {
            return iter->second;
        }

        auto data = FindUuidData( name );
        auto uuid_generate = __NEW_OBJECT__( KFUuidGenerate, data );
        _uuid_data_list[ name ] = uuid_generate;
        return uuid_generate;
    }

    uint64 KFUuid::STMake( const std::string& name, uint32 zone_id, uint32 worker_id, uint64 now_time )
    {
        auto uuid_data = CreateUuidGenerate( name );
        return uuid_data->Make( zone_id, worker_id, now_time );
    }

    uint64 KFUuid::MTMake( const std::string& name, uint32 zone_id, uint32 worker_id, uint64 now_time )
    {
        KFLocker locker( *_kf_mutex );
        return STMake( name, zone_id, worker_id, now_time );
    }

    uint32 KFUuid::STZoneId( const std::string& name, uint64 uuid )
    {
        auto uuid_data = CreateUuidGenerate( name );
        return uuid_data->ZoneId( uuid );
    }

    uint32 KFUuid::MTZoneId( const std::string& name, uint64 uuid )
    {
        KFLocker locker( *_kf_mutex );
        return STZoneId( name, uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuid::STParse( const std::string& name, uint64 uuid )
    {
        auto uuid_data = CreateUuidGenerate( name );
        return uuid_data->Parse( uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuid::MTParse( const std::string& name, uint64 uuid )
    {
        KFLocker locker( *_kf_mutex );
        return STParse( name, uuid );
    }
}

