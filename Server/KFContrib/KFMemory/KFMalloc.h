#ifndef __KF_MALLOC_H__
#define __KF_MALLOC_H__

#include "KFInclude.h"
#include "KFEnum.h"
#include "KFMemory/KFMemory.h"
#include "KFLogger/KFLogger.h"

namespace KFrame
{
#define __KF_NEW__( name, ... ) KFMalloc::Instance()->New< name >( __FUNCTION_LINE__, ##__VA_ARGS__ )
#define __KF_DELETE__( object ) KFMalloc::Instance()->Delete( object, __FUNCTION_LINE__ )

#define __KF_INT8__( length )\
    KFMalloc::Instance()->GetInt8( length, __FUNCTION_LINE__ )

#define __KF_UINT8__( length )\
    KFMalloc::Instance()->GetUInt8( length, __FUNCTION_LINE__ )

    // 对象池
#define __KF_CREATE__(name) KFMalloc::Instance()->Create<name >( __FUNCTION_LINE__ )
#define __KF_CREATE_BATCH__(name, batch) KFMalloc::Instance()->Create< name, batch >( __FUNCTION_LINE__ )
#define __KF_CREATE_PARAM1__(name, batch, param) KFMalloc::Instance()->Create< name, batch >( param, __FUNCTION_LINE__ )
#define __KF_CREATE_PARAM2__(name, batch, param1, param2) KFMalloc::Instance()->Create< name, batch >( param1, param2, __FUNCTION_LINE__ )
#define __KF_DESTROY__(name, object) KFMalloc::Instance()->Destroy< name >( object, __FUNCTION_LINE__ )

    // 内存池
#define __KF_MALLOC_BATCH__(name, size, batch) KFMalloc::Instance()->Malloc< name >( size, batch, __FUNCTION_LINE__ );
#define __KF_MALLOC__(name, size) __KF_MALLOC_BATCH__( name, size, 30 );
#define __KF_FREE__(name, memory, size) KFMalloc::Instance()->Free< name >( memory, size, __FUNCTION_LINE__ );
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class KFThreadBuffer;
    class KFMalloc
    {
    public:
        ~KFMalloc();

        // 创建
        static void Initialize( KFMalloc* kfmalloc );

        // 接口
        static KFMalloc* Instance();

        // 设置打开内存日志
        void SetLogMemoryOpen( bool open );
        ///////////////////////////////////////////////////////////////////////////////////
        template< class T, class ... P >
        inline T* New( const char* function, uint32 line, P&& ...params )
        {
            auto object = new T( std::forward<P>( params )... );
            KFLogger::LogMemory( KFLogger::Info, "[%s:%u] new object[0x%x:%s]!",
                                 function, line, object, typeid( T ).name() );
            return object;
        }

        template< class T >
        inline void Delete( T*& object, const char* function, uint32 line )
        {
            if ( object == nullptr )
            {
                return;
            }

            KFLogger::LogMemory( KFLogger::Info, "[%s:%u] delete object[0x%x:%s]!",
                                 function, line, object, typeid( T ).name() );

            delete object;
            object = nullptr;
        }

        ///////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////

        template< class T, uint32 batch = 30 >
        inline T * Create( const char* function, uint32 line )
        {
            auto memory = _kf_memory->Create( typeid( T ).name(), sizeof( T ), batch, function, line );
            return new ( memory ) T();
        }
        ///////////////////////////////////////////////////////////////////////////////////
        template< class T, uint32 batch = 30 >
        inline T * Create( uint32 param, const char* function, uint32 line )
        {
            auto memory = _kf_memory->Create( typeid( T ).name(), sizeof( T ), batch, function, line );
            return new ( memory ) T( param );
        }
        ///////////////////////////////////////////////////////////////////////////////////
        template< class T, uint32 batch = 30 >
        inline T * Create( uint32 param1, uint32 param2, const char* function, uint32 line )
        {
            auto memory = _kf_memory->Create( typeid( T ).name(), sizeof( T ), batch, function, line );
            return new ( memory ) T( param1, param2 );
        }
        ///////////////////////////////////////////////////////////////////////////////////
        template< class T >
        inline void Destroy( T* object, const char* function, uint32 line )
        {
            if ( object == nullptr )
            {
                return;
            }
            object->~T();
            _kf_memory->Delete( object, function, line );
        }
        ///////////////////////////////////////////////////////////////////////////////////
        template< class T >
        inline T* Malloc( uint32 size, uint32 batch, const char* function, uint32 line )
        {
            return reinterpret_cast< T* >( MallocMemory( size, batch, function, line ) );
        }

        template< class T >
        inline void Free( void* memory, uint32 size, const char* function, uint32 line )
        {
            FreeMemory( memory, size, function, line );
        }

        ///////////////////////////////////////////////////////////////////////////////////
        // 获得buff
        int8* GetInt8( uint32 length, const char* function, uint32 line );
        uint8* GetUInt8( uint32 length, const char* function, uint32 line );
        ///////////////////////////////////////////////////////////////////////////////////

        // 打印内存信息
        void PrintLogMemory();

    protected:
        KFMalloc();
        // 接口
        static KFMalloc* _kf_malloc;

        // 分配内存
        void* MallocMemory( uint32 size, uint32 batch, const char* function, uint32 line );

        // 释放内存
        void FreeMemory( void* memory, uint32 size, const char* function, uint32 line );

        // 计算最小的2的整数倍
        uint32 CalcNearest2NSize( uint32 size );

    private:
        // 内存分配器
        KFMemory* _kf_memory;

        // 直接new出来的内存
        std::set< void* > _new_memory;

        // 线程内存
        KFThreadBuffer* _kf_buffer;
    };
}

#endif