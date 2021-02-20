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
    uint64 KFUuidGenerate::Make( uint32 zone_id, uint32 worker_id, uint64 now_time )
    {
        // time
        auto time = ( now_time - _data->_start_time ) & _data->_max_time;
        if ( time != _last_time )
        {
            // 确保同1秒内序列号是递增的
            _sequence = 0;
            _last_time = time;
        }

        // 序列号
        _sequence = ( _sequence + 1 ) & _data->_max_seq;

        // app id
        zone_id &= _data->_max_zone;
        worker_id &= _data->_max_seq;
        return ( time << _data->_time_shift ) | ( zone_id << _data->_zone_shift ) | ( worker_id << _data->_worker_shift ) | _sequence;
    }

    uint32 KFUuidGenerate::ZoneId( uint64 uuid )
    {
        auto zone_id = ( ( uuid >> _data->_zone_shift ) & _data->_max_zone );
        return ( uint32 )zone_id;
    }

    std::tuple<uint64, uint32, uint32, uint32> KFUuidGenerate::Parse( uint64 uuid )
    {
        auto sequence = ( uuid & _data->_max_seq );
        auto worker_id = ( ( uuid >> _data->_worker_shift ) & _data->_max_worker );
        auto zone_id = ( ( uuid >> _data->_zone_shift ) & _data->_max_zone );
        auto time = ( ( uuid >> _data->_time_shift ) & _data->_max_time ) + _data->_start_time;
        return std::make_tuple( time, ( uint32 )zone_id, ( uint32 )worker_id, ( uint32 )sequence );
    }
}

