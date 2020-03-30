#include "KFrame.h"
#include "KFUuid.hpp"
#include "KFUuidData.hpp"
#include "KFUuidSetting.hpp"

namespace KFrame
{
    KFUuid::KFUuid()
    {
        _kf_mutex = new KFMutex();
    }

    KFUuid::~KFUuid()
    {
        delete _kf_mutex;
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

    void KFUuid::AddSetting( const std::string& name, uint64 projecttime, uint32 timebits, uint32 zonebits, uint32 workerbits, uint32 seqbits )
    {
        auto iter = _uuid_setting_list.find( name );
        if ( iter == _uuid_setting_list.end() )
        {
            auto kfsetting = new KFUuidSetting();
            iter = _uuid_setting_list.insert( std::make_pair( name, kfsetting ) ).first;
        }

        iter->second->InitSetting( projecttime, timebits, zonebits, workerbits, seqbits );
    }

    const KFUuidSetting* KFUuid::FindUuidSetting( const std::string& name )
    {
        auto iter = _uuid_setting_list.find( name );
        if ( iter == _uuid_setting_list.end() )
        {
            iter = _uuid_setting_list.find( __STRING__( normal ) );
        }

        return iter->second;
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFUuidData* KFUuid::CreateUuidData( const std::string& name )
    {
        auto iter = _uuid_data_list.find( name );
        if ( iter != _uuid_data_list.end() )
        {
            return iter->second;
        }

        auto kfsetting = FindUuidSetting( name );
        auto kfuuiddata = new KFUuidData( kfsetting );
        _uuid_data_list[ name ] = kfuuiddata;
        return kfuuiddata;
    }

    uint64 KFUuid::STMake( const std::string& name, uint32 zoneid, uint32 workerid, uint64 nowtime )
    {
        auto kfuuiddata = CreateUuidData( name );
        return kfuuiddata->Make( zoneid, workerid, nowtime );
    }

    uint64 KFUuid::MTMake( const std::string& name, uint32 zoneid, uint32 workerid, uint64 nowtime )
    {
        KFLocker locker( *_kf_mutex );
        return STMake( name, zoneid, workerid, nowtime );
    }

    uint32 KFUuid::STZoneId( const std::string& name, uint64 uuid )
    {
        auto kfuuiddata = CreateUuidData( name );
        return kfuuiddata->ZoneId( uuid );
    }

    uint32 KFUuid::MTZoneId( const std::string& name, uint64 uuid )
    {
        KFLocker locker( *_kf_mutex );
        return STZoneId( name, uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuid::STParse( const std::string& name, uint64 uuid )
    {
        auto kfuuiddata = CreateUuidData( name );
        return kfuuiddata->Parse( uuid );
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuid::MTParse( const std::string& name, uint64 uuid )
    {
        KFLocker locker( *_kf_mutex );
        return STParse( name, uuid );
    }
}

