#ifndef __MEMROY_LOGGER_H__
#define __MEMROY_LOGGER_H__

#include "KFDefine.h"

namespace KFrame
{
    class LogData
    {
    public:
        LogData() : _count( 0 ), _use_size( 0 ), _total_size( 0 )
        {

        }

        void AddData( uint64 size, uint64 totalsize );
        void DecData( uint64 size );

    public:
        uint32 _count;
        uint64 _use_size;
        uint64 _total_size;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    class LogBlock
    {
    public:
        void AddBlock( const std::string& type, uint64 size, uint64 totalsize );
        void DecBlock( const std::string& type, uint64 size );

    protected:
        LogData* FindData( const std::string& type );

    public:
        std::map< std::string, LogData* > _log_data;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFLogMemory
    {
    public:
        KFLogMemory();
        ~KFLogMemory();

        // 日志开关
        void SetOpen( bool open );
        bool IsOpen() const;

        LogBlock GetMTMemory();
        void AddMTMemory( const std::string& type, uint64 size, uint64 totalsize );
        void DecMTMemory( const std::string& type, uint64 size );
        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        LogBlock& GetSTMemory();
        void AddSTMemory( const std::string& type, uint64 size, uint64 totalsize );
        void DecSTMemory( const std::string& type, uint64 size );

    private:
        // 日志开关
        bool _open;

        // 多线程数据
        LogBlock _mt_log_block;

        // 单线程
        LogBlock _st_log_block;
    };
}

#endif
