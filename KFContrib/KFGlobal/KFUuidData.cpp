#include "KFUuidData.hpp"
#include "KFUuidSetting.hpp"

namespace KFrame
{
    KFUuidData::KFUuidData( const KFUuidSetting* kfsetting )
    {
        _kf_setting = kfsetting;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFUuidData::Make( uint32 zoneid, uint32 workerid, uint64 nowtime )
    {
        // time
        auto time = ( nowtime - _kf_setting->_start_time ) & _kf_setting->_max_time;
        if ( time != _last_time )
        {
            // 确保同1秒内序列号是递增的
            _sequence = 0;
            _last_time = time;
        }

        // 序列号
        _sequence = ( _sequence + 1 ) & _kf_setting->_max_seq;

        // appid
        zoneid &= _kf_setting->_max_zone;
        workerid &= _kf_setting->_max_seq;
        return ( time << _kf_setting->_time_shift ) | ( zoneid << _kf_setting->_zone_shift ) | ( workerid << _kf_setting->_worker_shift ) | _sequence;
    }

    uint32 KFUuidData::ZoneId( uint64 uuid )
    {
        auto zoneid = ( ( uuid >> _kf_setting->_zone_shift ) & _kf_setting->_max_zone );
        return ( uint32 )zoneid;
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuidData::Parse( uint64 uuid )
    {
        auto sequence = ( uuid & _kf_setting->_max_seq );
        auto workerid = ( ( uuid >> _kf_setting->_worker_shift ) & _kf_setting->_max_worker );
        auto zoneid = ( ( uuid >> _kf_setting->_zone_shift ) & _kf_setting->_max_zone );
        auto time = ( ( uuid >> _kf_setting->_time_shift ) & _kf_setting->_max_time ) + _kf_setting->_start_time;
        return std::make_tuple( time, ( uint32 )zoneid, ( uint32 )workerid, ( uint32 )sequence );
    }
}

