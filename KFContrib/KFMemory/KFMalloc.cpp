#include "KFrame.h"
#include "KFMalloc.h"
#include "Poco/SharedMemory.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////
    KFMalloc* KFMalloc::_kf_malloc = nullptr;
    KFMalloc::KFMalloc()
    {
        _memory_list_mutex = __NEW_OBJECT__( KFMutex );
        _share_memory_mutex = __NEW_OBJECT__( KFMutex );
        _thread_buffer_mutex = __NEW_OBJECT__( KFMutex );
    }

    KFMalloc::~KFMalloc()
    {
        for ( auto& iter : _int8_list )
        {
            free( iter.second._data );
        }
        _int8_list.clear();

        for ( auto& iter : _uint8_list )
        {
            free( iter.second._data );
        }
        _uint8_list.clear();

        __DELETE_OBJECT__( _memory_list_mutex );
        __DELETE_OBJECT__( _share_memory_mutex );
        __DELETE_OBJECT__( _thread_buffer_mutex );
    }

    void KFMalloc::Initialize( KFMalloc* malloc )
    {
        if ( malloc == nullptr )
        {
            malloc = new KFMalloc();
        }

        KFMalloc::_kf_malloc = malloc;
    }

    KFMalloc* KFMalloc::Instance()
    {
        return KFMalloc::_kf_malloc;
    }

    // 申请内存
    void KFMalloc::AddMemory( const std::string& name, void* memory, uint32 size )
    {
        if ( !_log_open )
        {
            return;
        }

        KFLocker locker( *_memory_list_mutex );

        // 加入内存列表
        MemoryData data;
        data._name = name;
        data._size = size;
        _memory_list[ memory ] = data;

        // 统计信息
        AddLogMemory( name, size );
    }

    // 查找内存
    void* KFMalloc::RemoveMemory( const std::string& name, void* memory, const char* function, uint32 line )
    {
        if ( !_log_open )
        {
            return memory;
        }

        KFLocker locker( *_memory_list_mutex );
        auto iter = _memory_list.find( memory );
        if ( iter == _memory_list.end() )
        {
            __LOG_ERROR_FUNCTION__( function, line, "name[{}] memory[{}] can't find", name, memory );
            return nullptr;
        }

        auto data = &iter->second;

        // 统计信息
        RemoveLogMemory( data->_name, data->_size );

        // 删除记录
        _memory_list.erase( iter );

        return memory;
    }

    void KFMalloc::AddLogMemory( const std::string& name, uint32 size )
    {
        auto iter = _log_data_list.find( name );
        if ( iter == _log_data_list.end() )
        {
            iter = _log_data_list.insert( std::make_pair( name, MemoryLogData() ) ).first;
        }

        auto log_data = &iter->second;
        log_data->_count++;
        log_data->_total_size += size;
    }

    void KFMalloc::RemoveLogMemory( const std::string& name, uint32 size )
    {
        auto iter = _log_data_list.find( name );
        if ( iter == _log_data_list.end() )
        {
            return;
        }

        auto log_data = &iter->second;
        log_data->_count--;
        log_data->_total_size -= size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    static uint64 _log_interval_time = 300000u;
    void KFMalloc::Run()
    {
        auto now_time = KFGlobal::Instance()->_game_time;
        if ( _next_log_time == 0u || now_time < _next_log_time )
        {
            return;
        }

        PrintMemoryLog();
        _next_log_time = now_time + _log_interval_time;
    }

    void KFMalloc::SetLogOpen( bool open )
    {
        _log_open = open;
        if ( _log_open )
        {
            _next_log_time = 1u;
        }
        else
        {
            _next_log_time = 0u;
        }
    }

#define MILLION_SIZE 1048576.0f
    void KFMalloc::PrintMemoryLog()
    {
        uint64 total_size = 0;
        __LOG_INFO__( "****************print memory start*****************" );
        {
            KFLocker locker( *_memory_list_mutex );
            for ( auto& iter : _log_data_list )
            {
                auto log_data = &iter.second;
                total_size += log_data->_total_size;
                __LOG_INFO__( "count[{}] total[{}] name[{}]", log_data->_count, log_data->_total_size, iter.first );
            }
        }
        __LOG_INFO__( "*********print memory end, total[{}]********", total_size );
    }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    int8* KFMalloc::GetInt8( uint32 length, const char* function, uint32 line )
    {
        auto thread_id = KFThread::GetID();

        KFLocker locker( *_thread_buffer_mutex );
        auto iter = _int8_list.find( thread_id );
        if ( iter == _int8_list.end() )
        {
            iter = _int8_list.insert( std::make_pair( thread_id, KFThreadBuffer< int8 >() ) ).first;
        }

        auto buffer = &iter->second;
        if ( buffer->_length < length )
        {
            buffer->_length = length;
            buffer->_data = reinterpret_cast< int8* >( realloc( buffer->_data, length ) );
            __LOG_INFO_FUNCTION__( function, line, "malloc int8[{}]", length );
        }

        return buffer->_data;
    }

    uint8* KFMalloc::GetUInt8( uint32 length, const char* function, uint32 line )
    {
        auto thread_id = KFThread::GetID();

        KFLocker locker( *_thread_buffer_mutex );
        auto iter = _uint8_list.find( thread_id );
        if ( iter == _uint8_list.end() )
        {
            iter = _uint8_list.insert( std::make_pair( thread_id, KFThreadBuffer< uint8 >() ) ).first;
        }

        auto buffer = &iter->second;
        if ( buffer->_length < length )
        {
            buffer->_length = length;
            buffer->_data = reinterpret_cast<uint8*>( realloc( buffer->_data, length ) );
            __LOG_INFO_FUNCTION__( function, line, "malloc uint8[{}]", thread_id, length );
        }

        return buffer->_data;
    }

    int8* KFMalloc::CreateShareMemory( const std::string& name, uint32 length )
    {
        KFLocker locker( *_share_memory_mutex );
        auto iter = _share_memory_list.find( name );
        if ( iter == _share_memory_list.end() )
        {
            KFShareMemory share_memory;
            share_memory._size = length;
            share_memory._data = new Poco::SharedMemory( name, length, Poco::SharedMemory::AM_WRITE );
            iter = _share_memory_list.insert( std::make_pair( name, share_memory ) ).first;
        }

        auto share_memory = &iter->second;
        if ( share_memory->_size != length )
        {
            __LOG_ERROR__( "share memory[{}] length[{} != {}] error", name, length );
        }

        auto memory = reinterpret_cast<Poco::SharedMemory*>( share_memory->_data );
        return memory->begin();
    }
}
