#include "KFUuidGenerate.hpp"
#include "KFUuidData.hpp"

namespace KFrame
{
    KFUuidGenerate::KFUuidGenerate( const KFUuidData* data )
    {
        _data = data;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////
    uint64 KFUuidGenerate::Make( uint32 zoneid, uint32 workerid, uint64 nowtime )
    {
        // time
        auto time = ( nowtime - _data->_start_time ) & _data->_max_time;
        if ( time != _last_time )
        {
            // 确保同1秒内序列号是递增的
            _sequence = 0;
            _last_time = time;
        }

        // 序列号
        _sequence = ( _sequence + 1 ) & _data->_max_seq;

        // appid
        zoneid &= _data->_max_zone;
        workerid &= _data->_max_seq;
        return ( time << _data->_time_shift ) | ( zoneid << _data->_zone_shift ) | ( workerid << _data->_worker_shift ) | _sequence;
    }

    uint32 KFUuidGenerate::ZoneId( uint64 uuid )
    {
        auto zoneid = ( ( uuid >> _data->_zone_shift ) & _data->_max_zone );
        return ( uint32 )zoneid;
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuidGenerate::Parse( uint64 uuid )
    {
        auto sequence = ( uuid & _data->_max_seq );
        auto workerid = ( ( uuid >> _data->_worker_shift ) & _data->_max_worker );
        auto zoneid = ( ( uuid >> _data->_zone_shift ) & _data->_max_zone );
        auto time = ( ( uuid >> _data->_time_shift ) & _data->_max_time ) + _data->_start_time;
        return std::make_tuple( time, ( uint32 )zoneid, ( uint32 )workerid, ( uint32 )sequence );
    }
}

