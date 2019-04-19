#ifndef __KF_MALLOC_H__
#define __KF_MALLOC_H__

#include "KFEnum.h"
#include "KFInclude.h"
#include "KFMacros.h"
#include "KFMemoryDefine.h"

namespace KFrame
{
    // 使用tcmalloc 代替原来的内存池
    // 对象
#define __KF_NEW__( name, ... ) KFMalloc::Instance()->New< name >( __FUNC_LINE__, ##__VA_ARGS__ )
#define __KF_DELETE__( name, object ) KFMalloc::Instance()->Delete< name >( object, __FUNC_LINE__ )

    // 内存
#define __KF_MALLOC__( name, size ) KFMalloc::Instance()->Malloc< name >( size, __FUNC_LINE__ );
#define __KF_FREE__( name, memory ) KFMalloc::Instance()->Free< name >( memory, __FUNC_LINE__ );

    // 线程大buff
#define __KF_INT8__( length )\
    KFMalloc::Instance()->GetInt8( length, __FUNC_LINE__ )

#define __KF_UINT8__( length )\
    KFMalloc::Instance()->GetUInt8( length, __FUNC_LINE__ )

    // 共享内存
#define __KF_SHARE_MEMORY__( name, length ) \
    KFMalloc::Instance()->CreateShareMemory( name, length )

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class KFMutex;
    class KFMalloc
    {
    public:
        ~KFMalloc();

        // 创建
        static void Initialize( KFMalloc* kfmalloc );

        // 接口
        static KFMalloc* Instance();
        ///////////////////////////////////////////////////////////////////////////////////
        // 申请内存
        template< class T >
        T* Malloc( uint32 size, const char* function, uint32 line )
        {
            auto* memory = malloc( size );
            AddMemory( typeid( T ).name(), memory, size );
            return reinterpret_cast< T* >( memory );
        }

        // 释放内存
        template< class T >
        void Free( void* memory, const char* function, uint32 line )
        {
            memory = RemoveMemory( typeid( T ).name(), memory, function, line );
            if ( memory == nullptr )
            {
                return;
            }

            free( memory );
        }
        ///////////////////////////////////////////////////////////////////////////////////////
        // 创建对象
        template< class T, class ... P >
        inline T* New( const char* function, uint32 line, P&& ...params )
        {
            auto size = static_cast< uint32 >( sizeof( T ) );
            auto* object = malloc( size );
            AddMemory( typeid( T ).name(), object, size );
            return new ( object ) T( std::forward< P >( params )... );
        }

        // 释放对象
        template< class T >
        inline void Delete( T* object, const char* function, uint32 line )
        {
            object = reinterpret_cast< T* >( RemoveMemory( typeid( T ).name(), object, function, line ) );
            if ( object == nullptr )
            {
                return;
            }

            object->~T();
            free( object );
        }

        ///////////////////////////////////////////////////////////////////////////////////
        // run
        void Run();

        // 设置打开内存日志
        void SetLogOpen( bool open );

        // 打印内存信息
        void PrintMemoryLog();

        // 获得线程buff
        int8* GetInt8( uint32 length, const char* function, uint32 line );
        uint8* GetUInt8( uint32 length, const char* function, uint32 line );

        // 创建共享内存
        int8* CreateShareMemory( const std::string& name, uint32 length );

    protected:
        KFMalloc();
        // 接口
        static KFMalloc* _kf_malloc;

        // 添加内存
        void AddMemory( const std::string& name, void* memory, uint32 size );

        // 查找内存
        void* RemoveMemory( const std::string& name, void* memory, const char* function, uint32 line );

        // 内存记录
        void AddLogMemory( const std::string& name, uint32 size );
        void RemoveLogMemory( const std::string& name, uint32 size );

    private:
        // 日志是否打开
        bool _log_open;
        // 打印日志时间
        uint64 _next_log_time = 0u;

        // 内存列表锁
        KFMutex* _memory_list_mutex;

        // 内存列表
        std::unordered_map< void*, MemoryData > _memory_list;

        // 统计信息
        std::unordered_map< std::string, MemoryLogData > _log_data_list;

        // 线程内存
        KFMutex* _thread_buffer_mutex;

        // 线程buffer列表
        std::unordered_map< uint32, KFThreadBuffer< int8 > > _int8_list;
        std::unordered_map< uint32, KFThreadBuffer< uint8 > > _uint8_list;

        // 共享内存
        KFMutex* _share_memory_mutex;
        std::unordered_map < std::string, KFShareMemory > _share_memory_list;
    };
}

#endif